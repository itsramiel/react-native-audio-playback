export enum IosAudioSessionCategory {
  Ambient,
  MultiRoute,
  PlayAndRecord,
  Playback,
  Record,
  SoloAmbient,
}

export enum AndroidAudioStreamUsage {
  Media,
  VoiceCommunication,
  VoiceCommunicationSignalling,
  Alarm,
  Notification,
  NotificationRingtone,
  NotificationEvent,
  AssistanceAccessibility,
  AssistanceNavigationGuidance,
  AssistanceSonification,
  Game,
  Assistant,
}

export enum StreamState {
  closed,
  initialized,
  open,
  paused,
}
