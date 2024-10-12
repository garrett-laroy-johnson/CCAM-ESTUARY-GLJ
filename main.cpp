#include "json/earth.h"
#include "daisysp.h"
#include <array>

json2daisy::DaisyEarth earth;
daisysp::Oscillator vco;
daisysp::Oscillator lfo;

static void EarthCallback(daisy::AudioHandle::InputBuffer in, 
            daisy::AudioHandle::OutputBuffer out, 
            size_t size) {
    earth.ProcessAllControls();
    earth.PostProcess();

    for (size_t i = 0; i < size; i++)
    {
        float value = vco.Process();
        out[0][i] = value;
        out[1][i] = value;
    }
}

static void CVOutCallback(uint16_t **out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = earth.knob1.GetRawValue();
        out[1][i] = (uint16_t)(lfo.Process() * 65535.0f);
    }
    earth.som.PrintLine("got %d %d", out[0][0], out[1][0]);
}

int main(void)
{
    earth.Init();
    earth.StartAudio(EarthCallback);
    earth.StartCV(CVOutCallback);

    vco.Init(earth.som.AudioSampleRate());
    lfo.Init(earth.som.dac.GetConfig().target_samplerate);

    earth.som.StartLog(false);
    earth.som.PrintLine("Hello world");

    while(1) {
        for (unsigned i = 0; i < earth.leds.size(); i++) {
            float value;
            if (earth.buttons[i]->Pressed()) {
                value = 0.0f;
            } else if (i < earth.knobs.size()) {
                value = earth.knobs[i]->Value() - 0.05;
            } else {
                value = 1.0f;
            }
            earth.leds[i]->Set(value);
        }

        daisy::System::Delay(1);
    }
}