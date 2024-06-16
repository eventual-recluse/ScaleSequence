/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2020 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"
#include "ScaleSequenceControls.hpp"
#include "Tunings.h"
#include "libMTSMaster.cpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Plugin to demonstrate File handling within DPF.
 */
class ScaleSequence : public Plugin
{
public:
    ScaleSequence()
        : Plugin(kParameterCount, 0, kStateCount),
          sampleRate(getSampleRate())
    {
        std::memset(fParameters, 0, sizeof(fParameters));
        
        // populate fParameters with defaults
        for (int32_t i = 0; i < kParameterCount; i++)
        {
            fParameters[i] = ParameterDefaults[i];
        }
        
        sampleRateChanged(sampleRate);
        
		current_scale = 0;
        tuning1 = Tunings::Tuning();
        tuning2 = Tunings::Tuning();
        tuning3 = Tunings::Tuning();
        tuning4 = Tunings::Tuning();
        
        //Fill frequency arrays with default frequencies from tuning1
        
        for (int32_t i = 0; i < 128; i++)
        {
            frequencies_in_hz[i] = tuning1.frequencyForMidiNote(i);;
            target_frequencies_in_hz[i] = tuning1.frequencyForMidiNote(i);;
        }
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
    const char* getLabel() const override
    {
        return "ScaleSequence";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "MTS-ESP Scale Sequencer.";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "eventual-recluse";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return "https://github.com/eventual-recluse/ScaleSequence";
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "GPL3";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(0, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('S', 'c', 'l', 'S');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /**
      Initialize the audio port @a index.@n
      This function will be called once, shortly after the plugin is created.
    */
    void initAudioPort(bool input, uint32_t index, AudioPort& port) override
    {
        // treat meter audio ports as stereo
        port.groupId = kPortGroupMono;

        // everything else is as default
        Plugin::initAudioPort(input, index, port);
    }

   /**
      Initialize the parameter @a index.
      This function will be called once, shortly after the plugin is created.
    */
    void initParameter(uint32_t index, Parameter& parameter) override
    {
        
        switch (index)
        {
        case kParameterMeasure:
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.name   = "Measure";
            parameter.symbol = "measure";
            parameter.enumValues.count = 2;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const values = new ParameterEnumerationValue[2];
                parameter.enumValues.values = values;

                values[0].label = "Beats";
                values[0].value = 0;
                values[1].label = "Bars";
                values[1].value = 1;
            }
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterMultiplier:
            parameter.name = "Multiplier";
            parameter.symbol = "multiplier";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterScaleGlide:
            parameter.name = "Scale Glide";
            parameter.symbol = "scaleGlide";
            parameter.hints = kParameterIsAutomatable|kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep1:
            parameter.name = "Step 1";
            parameter.symbol = "step1";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep2:
            parameter.name = "Step 2";
            parameter.symbol = "step2";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep3:
            parameter.name = "Step 3";
            parameter.symbol = "step3";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep4:
            parameter.name = "Step 4";
            parameter.symbol = "step4";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep5:
            parameter.name = "Step 5";
            parameter.symbol = "step5";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep6:
            parameter.name = "Step 6";
            parameter.symbol = "step6";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep7:
            parameter.name = "Step 7";
            parameter.symbol = "step7";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep8:
            parameter.name = "Step 8";
            parameter.symbol = "step8";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep9:
            parameter.name = "Step 9";
            parameter.symbol = "step9";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep10:
            parameter.name = "Step 10";
            parameter.symbol = "step10";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep11:
            parameter.name = "Step 11";
            parameter.symbol = "step11";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep12:
            parameter.name = "Step 12";
            parameter.symbol = "step12";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep13:
            parameter.name = "Step 13";
            parameter.symbol = "step13";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep14:
            parameter.name = "Step 14";
            parameter.symbol = "step14";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep15:
            parameter.name = "Step 15";
            parameter.symbol = "step15";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterStep16:
            parameter.name = "Step 16";
            parameter.symbol = "step16";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
        case kParameterOffset:
            parameter.name = "Offset";
            parameter.symbol = "offset";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = ParameterDefaults[index];
            break;
		case kParameterCurrentStep:
			parameter.name = "Current Step";
            parameter.symbol = "currentstep";
            parameter.hints = kParameterIsOutput;
			break;
        }
    }

   /**
      Initialize the state @a index.@n
      This function will be called once, shortly after the plugin is created.
    */
    void initState(uint32_t index, State& state) override
    {
        switch (index)
        {
        case kStateFileSCL1:
            state.key = "scl_file_1";
            state.label = "SCL File 1";
            break;
        case kStateFileSCL2:
            state.key = "scl_file_2";
            state.label = "SCL File 2";
            break;
        case kStateFileSCL3:
            state.key = "scl_file_3";
            state.label = "SCL File 3";
            break;
        case kStateFileSCL4:
            state.key = "scl_file_4";
            state.label = "SCL File 4";
            break;
        case kStateFileKBM1:
            state.key = "kbm_file_1";
            state.label = "KBM File 1";
            break;
        case kStateFileKBM2:
            state.key = "kbm_file_2";
            state.label = "KBM File 2";
            break;
        case kStateFileKBM3:
            state.key = "kbm_file_3";
            state.label = "KBM File 3";
            break;
        case kStateFileKBM4:
            state.key = "kbm_file_4";
            state.label = "KBM File 4";
            break;
        }

        state.hints = kStateIsFilenamePath;
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

   /**
      Get the current value of a parameter.
      The host may call this function from any context, including realtime processing.
    */
    float getParameterValue(uint32_t index) const override
    {
        return fParameters[index];
    }

   /**
      Change a parameter value.@n
      The host may call this function from any context, including realtime processing.

      This function will only be called for parameter inputs.
    */
    void setParameterValue(uint32_t index, float value) override
    {
		fParameters[index] = value;
	}

   /**
      Change an internal state @a key to @a value.
    */
    void setState(const char* key, const char* value) override
    {
		// note: internal states seem to get set as soon as file chosen by file dialog, and could end up being anything
		
        //d_stdout("DSP setState %s %s", key, value);
        int fileId = -1;

        /**/ if (std::strcmp(key, "scl_file_1") == 0)
        {
		    fileId = 0;
		}
        else if (std::strcmp(key, "scl_file_2") == 0)
        {   
			fileId = 1;
		}
        else if (std::strcmp(key, "scl_file_3") == 0)
	    {
            fileId = 2;
        }
        else if (std::strcmp(key, "scl_file_4") == 0)
	    {
            fileId = 3;
        }
        else if (std::strcmp(key, "kbm_file_1") == 0)
	    {
            fileId = 4;
        }
        else if (std::strcmp(key, "kbm_file_2") == 0)
	    {
            fileId = 5;
        }
        else if (std::strcmp(key, "kbm_file_3") == 0)
	    {
            fileId = 6;
        }
        else if (std::strcmp(key, "kbm_file_4") == 0)
	    {
            fileId = 7;
        }
        
        if (fileId == -1)
            return;

        if (FILE* const fh = fopen(value, "r"))
        {
			
			if (!value)
			{    
			    fclose(fh);
			    return; 
			}
			
			//d_stdout(value);
			
			//String filename(value);
			

			if (fileId == 0)
			{	
				loadScl(tuning1, value);
			}
			else if (fileId == 1)
			{
				loadScl(tuning2, value);
			}
			else if (fileId == 2)
			{
				loadScl(tuning3, value);
			}	
			else if (fileId == 3)
			{	
				loadScl(tuning4, value);
			}		
			else if (fileId == 4)
			{
				loadKbm(tuning1, value);
			}
			else if (fileId == 5)
			{
				loadKbm(tuning2, value);
			}
			else if (fileId == 6)
			{
				loadKbm(tuning3, value);
			}
			else if (fileId == 7)
			{
				loadKbm(tuning4, value);
			}
			
            fclose(fh);
        }
    }
    
    void loadScl(Tunings::Tuning & tn, const char* value)
    {
		String filename(value);
		auto k = tn.keyboardMapping;
		
		if (filename.endsWith(".scl"))
		{
			try
			{   auto s = Tunings::readSCLFile(value);
				tn = Tunings::Tuning(s, k);
				//d_stdout("ScaleSequence: tuning set to %s", value);
			}
			catch (const std::exception& e)
			{
				tn = Tunings::Tuning();
				d_stdout("ScaleSequence:Exception when setting tuning");
				d_stdout(e.what());
			}
		}
		else
		{
			auto s = Tunings::Tuning().scale;
			tn = Tunings::Tuning(s, k);
			//d_stdout("ScaleSequence: tuning scl reset");
		}
	}
	
	void loadKbm(Tunings::Tuning & tn, const char* value)
	{
		String filename(value);
		auto s = tn.scale;
		if (filename.endsWith(".kbm"))
		{
			try
			{
				auto k = Tunings::readKBMFile(value);
				tn = Tunings::Tuning(s, k);
				//d_stdout("ScaleSequence: tuning set to %s", value);
			}
			catch (const std::exception& e)
			{
				tn = Tunings::Tuning();
				d_stdout("ScaleSequence:Exception when setting tuning");
				d_stdout(e.what());
			}
		}
		else
		{
			auto k = Tunings::Tuning().keyboardMapping;
			tn = Tunings::Tuning(s, k);
			//d_stdout("ScaleSequence: tuning kbm reset");
		}
	}

    /* --------------------------------------------------------------------------------------------------------
    * Activate / Deactivate */
    
    void activate() override
    {
	    if (MTS_CanRegisterMaster())
			MTS_RegisterMaster();
		current_scale = 0;
	}
	
    void deactivate() override
    {
        MTS_DeregisterMaster();
    }
    
   /* --------------------------------------------------------------------------------------------------------
    * Audio/MIDI Processing */

   /**
      Run/process function for plugins without MIDI input.
      @note Some parameters might be null if there are no audio inputs or outputs.
    */
    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
       
            
        const TimePosition& timePos(getTimePosition());
        
        double beats_per_bar = timePos.bbt.beatsPerBar;
        // In DISTRHO DPF, the first bar == 1. But our calculations require first bar == 0
        double bar = timePos.bbt.bar - 1;
        // In DISTRHO DPF, the first beat of the bar == 1. Our calculations require first beat of the bar == 0
        double beat = timePos.bbt.beat - 1;
        double beatFraction   = timePos.bbt.tick / timePos.bbt.ticksPerBeat;
                
        double beatsFromStart = (bar * beats_per_bar) + beat + beatFraction;
                
        // Offset. Might cause weirdness at the start of the track. But stepIndex below should be ignored if less than zero.
        if (fParameters[kParameterMeasure] == 0) // using beats
            beatsFromStart -= fParameters[kParameterOffset];
        else if (fParameters[kParameterMeasure] == 1) //using bars
            bar -= fParameters[kParameterOffset];
        
        
        int32_t stepIndex = 0;
        
        // Which step are we on?
        if (fParameters[kParameterMeasure] == 0) // using beats
            stepIndex = static_cast<int32_t>(std::floor(beatsFromStart / fParameters[kParameterMultiplier])) % 16;
        else if (fParameters[kParameterMeasure] == 1) // using bars
            stepIndex = static_cast<int32_t>(std::floor(bar / fParameters[kParameterMultiplier])) % 16;
        
        // Set current step parameter for UI feedback
        fParameters[kParameterCurrentStep] = static_cast<float>((stepIndex + 1) * 0.0625f);
        
        int32_t stepScale = 0;
        
        // What should the scale be for this step?
        switch (stepIndex)
        {
        case 0:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep1]);
            break;
        case 1:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep2]);
            break;
        case 2:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep3]);
            break;
        case 3:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep4]);
            break;
        case 4:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep5]);
            break;
        case 5:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep6]);
            break;
        case 6:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep7]);
            break;
        case 7:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep8]);
            break;
        case 8:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep9]);
            break;
        case 9:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep10]);
            break;
        case 10:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep11]);
            break;
        case 11:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep12]);
            break;
        case 12:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep13]);
            break;
        case 13:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep14]);
            break;
        case 14:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep15]);
            break;
        case 15:
            stepScale = static_cast<uint32_t>(fParameters[kParameterStep16]);
            break;
        default:
            break;
		}
		
		// Switch scale if necessary
		// if stepScale is still 0 it will be ignored, and the tuning won't change
        if (stepScale != current_scale)
        {
			switch (stepScale)
			{
			case 1:
			{
				for (int32_t i = 0; i < 128; i++)
				{
					target_frequencies_in_hz[i] = tuning1.frequencyForMidiNote(i);
				}
				break;
			}
			case 2:
			{
				for (int32_t i = 0; i < 128; i++)
				{
					target_frequencies_in_hz[i] = tuning2.frequencyForMidiNote(i);
				}
				break;
			}
			case 3:
			{
				for (int32_t i = 0; i < 128; i++)
				{
					target_frequencies_in_hz[i] = tuning3.frequencyForMidiNote(i);
				}
				break;
			}
			case 4:
			{
				for (int32_t i = 0; i < 128; i++)
				{
					target_frequencies_in_hz[i] = tuning4.frequencyForMidiNote(i);
				}
				break;
			}
			default:
                break;
			}
	
		}
		
		// Scale glide, continuous tuning. Currently done via division of the remaining difference to target
		// for every frame
	        for (uint32_t fr = 0; fr < frames; ++fr)
		{
			for (int32_t i = 0; i < 128; i++)
			{
				double difference = target_frequencies_in_hz[i] - frequencies_in_hz[i];
				if (std::fabs(difference) < 0.0001f)
					frequencies_in_hz[i] = target_frequencies_in_hz[i];
				else
					frequencies_in_hz[i] = frequencies_in_hz[i] + (difference / (fParameters[kParameterScaleGlide] * 1000.0));
			}
			// Set MTS-ESP Scale
			MTS_SetNoteTunings(frequencies_in_hz);
		}
    }
    

    // -------------------------------------------------------------------------------------------------------

private:
    float sampleRate;

    float fParameters[kParameterCount];
    Tunings::Tuning tuning1, tuning2, tuning3, tuning4;
    
    double frequencies_in_hz[128];
    double target_frequencies_in_hz[128];
    uint32_t current_scale;

   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleSequence)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new ScaleSequence();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
