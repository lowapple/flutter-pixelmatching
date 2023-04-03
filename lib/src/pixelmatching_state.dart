enum PixelMatchingState {
  // The pixel matching is not initialized.
  notInitialized,
  // target image is not set.
  waitingForTarget,
  // query image is not set.
  readyToProcess,
  // is processing.
  processing,
}
