package com.audioplayback.models

import android.content.Context
import android.os.ParcelFileDescriptor
import android.util.Log
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.util.UUID

class FileDescriptorProps(val id: Int, val length: Int, val offset: Int) {
  companion object {
    fun fromLocalResource(context: Context,resourceName: String): FileDescriptorProps {
      val resourceId = context.resources.getIdentifier(resourceName, "raw", context.packageName)
      val assetFileDescriptor = context.resources.openRawResourceFd(resourceId)
      val fileOffset = assetFileDescriptor.startOffset.toInt()
      val fileLength = assetFileDescriptor.length.toInt()

      val fileDescriptorId = assetFileDescriptor.parcelFileDescriptor.detachFd()
      return FileDescriptorProps(fileDescriptorId, fileLength, fileOffset)
    }

    fun getFileDescriptorPropsFromUrl(context: Context,url: URL): FileDescriptorProps? {
      val connection = url.openConnection() as HttpURLConnection
      connection.requestMethod = "GET"
      connection.connect()

      if (connection.responseCode == HttpURLConnection.HTTP_OK) {
        // Save the downloaded file to a temporary file
        val inputStream = connection.inputStream
        val tempFile =
          File(context.cacheDir, "tempSoundFile_${UUID.randomUUID()}")
        tempFile.outputStream().use { outputStream ->
          inputStream.copyTo(outputStream)
        }
        inputStream.close()

        // Get file descriptor and pass to native
        val assetFileDescriptor =
          ParcelFileDescriptor.open(tempFile, ParcelFileDescriptor.MODE_READ_ONLY)
        val fileDescriptorId = assetFileDescriptor.detachFd()

        tempFile.deleteOnExit()
        return FileDescriptorProps(fileDescriptorId, tempFile.length().toInt(), 0)
      } else {
        Log.e(
          "OboeModule",
          "Failed to load sound from Metro server. HTTP response code: ${connection.responseCode}"
        )
        return null
      }
    }

  }
}
