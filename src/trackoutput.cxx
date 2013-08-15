
#include "trackoutput.hxx"

TrackOutput::TrackOutput(int t, AudioProcessor* ap) :
  AudioProcessor(),
  track(t),
  previousInChain(ap),
  dbMeter(44100)
{
  //printf("trackOutput ID: %i, ap = ", track );
  //std::cout << ap << std::endl;
  
  //_trackBuffer.resize( MAX_BUFFER_SIZE );
  
  // UI update
  uiUpdateConstant = 44100 / 30;
  uiUpdateCounter  = 44100 / 30;
  
  _toReverb        = 0.0;
  _toMaster        = 0.8;
  _toSidechain     = 0.0;
  _toPostSidechain = 0.0;
}


void TrackOutput::setMaster(float value)
{
  _toMaster = value;
}

float TrackOutput::getMaster()
{
  return _toMaster;
}


void TrackOutput::setSend( int send, float value )
{
  switch( send )
  {
    case SEND_REV:
        _toReverb = value;
        break;
    case SEND_SIDE:
        _toSidechain = value;
        break;
    case SEND_POST:
        _toPostSidechain = value;
        break;
  }
  
}

void TrackOutput::process(int nframes, Buffers* buffers)
{
  // zero track buffer
  memset( &_trackBuffer[0], 0, nframes );
  
  buffers->audio[Buffers::TRACK_0 + track] = &_trackBuffer[0];
  previousInChain->process( nframes, buffers );
  
  
  // run the meter
  float* buf = &_trackBuffer[0];
  dbMeter.process( nframes, buf, buf );
  
  if (uiUpdateCounter > uiUpdateConstant )
  {
    // FIXME: should be using ControllerUpdater
    EventTrackSignalLevel e( track, dbMeter.getLeftDB() * _toMaster, dbMeter.getRightDB() * _toMaster );
    writeToGuiRingbuffer( &e );
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  
  // copy audio data into reverb / sidechain / master buffers
  float* reverb        = buffers->audio[Buffers::REVERB];
  float* sidechain     = buffers->audio[Buffers::SIDECHAIN];
  float* postSidechain = buffers->audio[Buffers::POST_SIDECHAIN];
  
  float* masterL       = buffers->audio[Buffers::MASTER_OUT_L];
  float* masterR       = buffers->audio[Buffers::MASTER_OUT_R];
  
  for(int i = 0; i < nframes; i++)
  {
    float tmp = _trackBuffer[i];
    
    masterL[i]       += tmp * _toMaster;
    masterR[i]       += tmp * _toMaster;
    
    masterL++;
    masterR++;
    
    *reverb++        += tmp * _toReverb;
    *sidechain++     += tmp * _toSidechain;
    *postSidechain++ += tmp * _toPostSidechain;
  }
}

TrackOutput::~TrackOutput()
{
}
