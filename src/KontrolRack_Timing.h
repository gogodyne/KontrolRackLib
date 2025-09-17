// (c) 2025 gogodyne
#ifndef KontrolRack_Timing_h
#define KontrolRack_Timing_h

namespace KontrolRack {

////////////////////////////////////////////////////////////////////////////////
typedef unsigned long timing_t;
typedef uint16_t fps_t;
class Timing
{
public:
  class Timer
  {
  public:
    Timing& timing;
    timing_t duration = 0;
    timing_t timeout = 0;

    Timer(Timing& timing, timing_t duration = 1000)
    : timing(timing)
    , duration(duration)
    {}
    void start()                    { timeout = timing.ms + duration; }
    void start(timing_t duration)   { timeout = timing.ms + (this->duration = duration); }
    void end()                      { timeout = timing.ms; }// set to full progress
    void stop()                     { timeout = 0; }// reset; no progress
    bool didStart()                 { return timeout > 0; }
    float progress()                { return isDone() ? 1 : duration ? (1.f - (((float)timeout - (float)timing.ms) / (float)duration)) : 0; }
    bool isDone()                   { return didStart() && (timeout <= timing.ms); }
  };

  timing_t frame = 0;
  timing_t ms = 0;
  timing_t delta = 0;
  float fpsEstimate = 0;
  fps_t fps = 30;
  timing_t nextFrameMs = 0;
  bool isTick = false;

  virtual void begin(fps_t fps)
  {
    this->fps = fps;
  }
  
  bool loop()
  {
    timing_t now = millis();
    isTick = false;
    if (now >= nextFrameMs)
    {
      ++frame;
      nextFrameMs = now + (1000. / (float)fps);
      isTick = true;
    }
    delta = now - ms;
    fpsEstimate = delta ? (1.f / ((float)delta * .001f)) : 1.f;
    ms = now;

    return isTick;
  }

  // 50% duty cycle
  bool isHz(float hz)
  {
    return (hz ? 1 & (int)(ms * .002 * hz) : 1);
  }
};

}// namespace KontrolRack

#endif// KontrolRack_Timing_h
