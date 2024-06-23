/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2021 Filipe Coelho <falktx@falktx.com>
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

#include <string>
#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "extra/String.hpp"
#include "ScaleSequenceControls.hpp"
#include "BrunoAceFont.hpp"
#include "BrunoAceSCFont.hpp"
#include "LektonRegularFont.hpp"
#include "Tunings.h"

START_NAMESPACE_DISTRHO

const char* kStateKeys[kStateCount] = {
    "scl_file_1",
    "scl_file_2",
    "scl_file_3",
    "scl_file_4",
    "kbm_file_1",
    "kbm_file_2",
    "kbm_file_3",
    "kbm_file_4",
};

// --------------------------------------------------------------------------------------------------------------------

class ScaleSequenceUI : public UI
{
public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    ScaleSequenceUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);
        
        // hide handle if UI is resizable
        if (isResizable())
            fResizeHandle.hide();
            
        std::memset(fParameters, 0, sizeof(fParameters));
        
        // populate fParameters with defaults
        for (int32_t i = 0; i < kParameterCount; i++)
        {
            fParameters[i] = ParameterDefaults[i];
        }
                
        for (int32_t i = 0; i < kStateCount; i++)
        {
			String d(" ");
			fFileBaseName[i] = d;
		}
		
 		utuning1 = Tunings::Tuning(); 
		utuning2 = Tunings::Tuning(); 
		utuning3 = Tunings::Tuning(); 
		utuning4 = Tunings::Tuning(); 
		
		ui_multiplier = static_cast<int>(ParameterDefaults[kParameterMultiplier]);
		ui_loopPoint = static_cast<int>(ParameterDefaults[kParameterLoopPoint]);
		
        // account for scaling
        scale_factor = getScaleFactor();
        if (scale_factor == 0) {scale_factor = 1.0;}
        
        UI_COLUMN_WIDTH = 312 * scale_factor;
        
        // Setup fonts
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        
        brunoAceFont = AddBrunoAceFont(scale_factor);
        lektonRegularFont = AddLektonRegularFont(scale_factor);
        brunoAceStepFont = AddBrunoAceFont(scale_factor*1.3);
        brunoAceSCFont = AddBrunoAceSCFont(scale_factor);
        
        show_error_popup = false;
        errorText.clear();
        
        // Set style and colours
        ImGuiStyle& uistyle = ImGui::GetStyle();
        
        uistyle.GrabRounding = 3.6f;
        
        ImVec4 light_orange (0.965f, 0.714f, 0.282f, 0.8f);
        ImVec4 medium_orange (1.0f, 0.63f, 0.0f, 0.9f);
        ImVec4 pale_orange (0.965f, 0.851f, 0.655f, 0.96f);
        ImVec4 pale (0.98f, 0.98f, 0.98f, 1.00f);
        ImVec4 black (0.0f, 0.0f, 0.0f, 1.00f);
        ImVec4 pale_trans (1.0f, 1.0f, 1.0f, 0.7f);
        ImVec4 transparent (0.0f, 0.0f, 0.0f, 0.00f);
        ImVec4 grey (0.50f, 0.50f, 0.50f, 1.0f);
        
        ImVec4 light_peach (0.95f, 0.33f, 0.14f, 0.36f);
        ImVec4 peach (0.95f, 0.33f, 0.14f, 0.47f);
        ImVec4 deep_peach (0.97f, 0.31f, 0.13f, 0.81f);
        ImVec4 light_orange_opaque (1.00f, 0.79f, 0.18f, 1.0f);

        uistyle.Colors[ImGuiCol_Text] = black;
        uistyle.Colors[ImGuiCol_TextDisabled] = black;
        uistyle.Colors[ImGuiCol_WindowBg] = pale;
        uistyle.Colors[ImGuiCol_ChildBg] = transparent;
        uistyle.Colors[ImGuiCol_PopupBg] = pale;
        uistyle.Colors[ImGuiCol_Border] = grey;
        uistyle.Colors[ImGuiCol_BorderShadow] = transparent;
        uistyle.Colors[ImGuiCol_FrameBg] = light_orange;
        uistyle.Colors[ImGuiCol_FrameBgHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_FrameBgActive] = pale_orange;
        uistyle.Colors[ImGuiCol_TitleBg] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_TitleBgActive] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_TitleBgCollapsed] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_MenuBarBg] = light_orange;
        uistyle.Colors[ImGuiCol_ScrollbarBg] = light_orange;
        uistyle.Colors[ImGuiCol_ScrollbarGrab] = pale_trans;
        uistyle.Colors[ImGuiCol_ScrollbarGrabHovered] = peach;
        uistyle.Colors[ImGuiCol_ScrollbarGrabActive] = deep_peach;
        uistyle.Colors[ImGuiCol_CheckMark] = pale;
        uistyle.Colors[ImGuiCol_SliderGrab] = pale_trans;
        uistyle.Colors[ImGuiCol_SliderGrabActive] = pale;
        uistyle.Colors[ImGuiCol_Button] = light_orange;
        uistyle.Colors[ImGuiCol_ButtonHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_ButtonActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Header] = light_orange;
        uistyle.Colors[ImGuiCol_HeaderHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_HeaderActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Separator] = pale;
        uistyle.Colors[ImGuiCol_SeparatorHovered] = pale;
        uistyle.Colors[ImGuiCol_SeparatorActive] = pale;
        uistyle.Colors[ImGuiCol_ResizeGrip] = light_orange;
        uistyle.Colors[ImGuiCol_ResizeGripHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_ResizeGripActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Tab] = black;
        uistyle.Colors[ImGuiCol_TabHovered] = black;
        uistyle.Colors[ImGuiCol_TabActive] = black;
        uistyle.Colors[ImGuiCol_TabUnfocused] = black;
        uistyle.Colors[ImGuiCol_TabUnfocusedActive] = black;
        uistyle.Colors[ImGuiCol_PlotLines] = black;
        uistyle.Colors[ImGuiCol_PlotLinesHovered] = black;
        uistyle.Colors[ImGuiCol_PlotHistogram] = black;
        uistyle.Colors[ImGuiCol_PlotHistogramHovered] = black;
        uistyle.Colors[ImGuiCol_TableHeaderBg] = black;
        uistyle.Colors[ImGuiCol_TableBorderStrong] = black;
        uistyle.Colors[ImGuiCol_TableBorderLight] = black;
        uistyle.Colors[ImGuiCol_TableRowBg] = black;
        uistyle.Colors[ImGuiCol_TableRowBgAlt] = black;
        uistyle.Colors[ImGuiCol_TextSelectedBg] = medium_orange;
        uistyle.Colors[ImGuiCol_DragDropTarget] = black;
        uistyle.Colors[ImGuiCol_NavHighlight] = black;
        uistyle.Colors[ImGuiCol_NavWindowingHighlight] = black;
        uistyle.Colors[ImGuiCol_NavWindowingDimBg] = black;
        uistyle.Colors[ImGuiCol_ModalWindowDimBg] =  black;
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        fParameters[index] = value;
        
        // update ui variables for SliderInt and CheckBox widgets
        // Check for File load success
        switch (index)
        {
        case kParameterMultiplier:
            ui_multiplier = static_cast<int>(fParameters[kParameterMultiplier]);
            break;
        case kParameterLoopPoint:
            ui_loopPoint = static_cast<int>(fParameters[kParameterLoopPoint]);
            break;
		
        default:
            break;
        }
        
        repaint();
    }

   /**
      A state has changed on the plugin side.@n
      This is called by the host to inform the UI about state changes.
    */
    void stateChanged(const char* key, const char* value) override
    {
		States stateId = kStateCount;

        /**/ if (std::strcmp(key, "scl_file_1") == 0)
            stateId = kStateFileSCL1;
        else if (std::strcmp(key, "scl_file_2") == 0)
            stateId = kStateFileSCL2;
        else if (std::strcmp(key, "scl_file_3") == 0)
            stateId = kStateFileSCL3;
        else if (std::strcmp(key, "scl_file_4") == 0)
            stateId = kStateFileSCL4;
        else if (std::strcmp(key, "kbm_file_1") == 0)
            stateId = kStateFileKBM1;
        else if (std::strcmp(key, "kbm_file_2") == 0)
            stateId = kStateFileKBM2;
        else if (std::strcmp(key, "kbm_file_3") == 0)
            stateId = kStateFileKBM3;
        else if (std::strcmp(key, "kbm_file_4") == 0)
            stateId = kStateFileKBM4;

        if (stateId == kStateCount)
            return;
        
        fState[stateId] = value;
        
        // NOTE: We will mirror what's happening on the DSP side

        if (stateId == 0)
		{	
			checkScl(utuning1, value, stateId);
		}
		else if (stateId == 1)
		{
			checkScl(utuning2, value, stateId);
		}
		else if (stateId == 2)
		{
			checkScl(utuning3, value, stateId);
		}	
		else if (stateId == 3)
		{	
			checkScl(utuning4, value, stateId);
		}		
		else if (stateId == 4)
		{
			checkKbm(utuning1, value, stateId);
		}
		else if (stateId == 5)
		{
			checkKbm(utuning2, value, stateId);
		}
		else if (stateId == 6)
		{
			checkKbm(utuning3, value, stateId);
		}
		else if (stateId == 7)
		{
			checkKbm(utuning4, value, stateId);
		}
	    
        repaint();
    }
    
    void checkScl(Tunings::Tuning & tn, const char* value, const States & stateId)
    {
		String filename(value);
		auto k = tn.keyboardMapping;
		
		if (filename.endsWith(".scl"))
		{
			try
			{   auto s = Tunings::readSCLFile(value);
				tn = Tunings::Tuning(s, k);
                const char *a = tn.scale.name.c_str();
                fFileBaseName[stateId] = getFileBaseName(a);
			}
			catch (const std::exception& e)
			{
				tn = Tunings::Tuning();
				String noScl("Standard SCL tuning");
                String noKbm("Standard KBM mapping");
                fFileBaseName[stateId] = noScl;
                fFileBaseName[stateId+4] = noKbm;
                String tuningError(e.what());
                errorText = "Tuning error:\n" + tuningError + "\nScale reset to standard tuning and mapping.";
                setState(kStateKeys[stateId], "");
				setState(kStateKeys[stateId+4], "");
                show_error_popup = true;
                //d_stdout("UI:");
                //d_stdout(e.what());
			}
		}
		else
		{
			auto s = Tunings::Tuning().scale;
			tn = Tunings::Tuning(s, k);
			String noScl("Standard SCL tuning");
			fFileBaseName[stateId] = noScl;
			
			if (filename.isNotEmpty())
			{
				errorText = "Not a .scl file.\nSCL tuning reset to standard.";
				show_error_popup = true;
				setState(kStateKeys[stateId], "");
			}
		}
	}
	
	void checkKbm(Tunings::Tuning & tn, const char* value, const States & stateId)
	{
		String filename(value);
		auto s = tn.scale;
		if (filename.endsWith(".kbm"))
		{
			try
			{
				auto k = Tunings::readKBMFile(value);
				tn = Tunings::Tuning(s, k);
				const char *a = tn.keyboardMapping.name.c_str();
                fFileBaseName[stateId] = getFileBaseName(a);
			}
			catch (const std::exception& e)
			{
				tn = Tunings::Tuning();
				String noScl("Standard SCL tuning");
                String noKbm("Standard KBM mapping");
                fFileBaseName[stateId - 4] = noScl;
                fFileBaseName[stateId] = noKbm;
                String tuningError(e.what());
                errorText = "Tuning error:\n" + tuningError + "\nScale reset to standard tuning and mapping.";
                setState(kStateKeys[stateId], "");
				setState(kStateKeys[stateId-4], "");
                show_error_popup = true;
                //d_stdout("UI:");
                //d_stdout(e.what());
			}
		}
		else
		{
			auto k = Tunings::Tuning().keyboardMapping;
			tn = Tunings::Tuning(s, k);
			String noKbm("Standard KBM mapping");
			fFileBaseName[stateId] = noKbm;
			
			if (filename.isNotEmpty())
			{
				errorText = "Not a .kbm file.\nKBM mapping reset to standard.";
				show_error_popup = true;
				setState(kStateKeys[stateId], "");
			}
			
		}
	}
	
    String getFileBaseName(const char* value)
    {
        std::string p(value);
        // TODO Find a better way to derive the file name!
        p = p.substr(p.find_last_of("/\\") +1);
        const char *a = p.c_str();
        String baseName(a);
        return baseName;
    }
    
    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
		const float width = getWidth();
        const float height = getHeight();
        const float margin = 10.0f * getScaleFactor();
        
        ImVec4 step_highlight_color(0.95f, 0.33f, 0.14f, 0.47f);

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        if (ImGui::Begin("ScaleSequence", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar))
        {
			ImGui::BeginChild("title pane", ImVec2(0, ImGui::GetFontSize() * 3));
            
            ImGui::PushFont(brunoAceSCFont);
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##title_label", "ScaleSequence");
            
            //ImGui::PopStyleColor();
            ImGui::PopFont();
            
            ImGui::EndChild(); // title pane
            
            ImGui::BeginChild("top pane", ImVec2(0, 300 * scale_factor)); // top pane holds two colums
            
            ImGui::BeginChild("left pane", ImVec2(UI_COLUMN_WIDTH, 0));
            
            //---
            
            ImGui::BeginChild("scale one pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 8.0f), true);
            
            ImGui::LabelText("##scale_1_label", "SCALE 1");
            
			if (ImGui::Button("Open SCL File##1"))
			{
				requestStateFile(kStateKeys[kStateFileSCL1]);
			}
			
			ImGui::SameLine(); 
			
			if (ImGui::Button("Open KBM File##1"))
			{
				requestStateFile(kStateKeys[kStateFileKBM1]);
			}
			
			ImGui::PushFont(lektonRegularFont);
			ImGui::PushItemWidth(-1);
			ImGui::LabelText("##scale_1_scl", fFileBaseName[kStateFileSCL1]);
			ImGui::LabelText("##scale_1_kbm", fFileBaseName[kStateFileKBM1]);
			ImGui::PopItemWidth();
			ImGui::PopFont();
            
            ImGui::EndChild(); // scale one pane
            
 
            //---
            
            ImGui::BeginChild("scale three pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 8.0f), true);
            
            ImGui::LabelText("##scale_3_label", "SCALE 3");
            
			if (ImGui::Button("Open SCL File##3"))
			{
				requestStateFile(kStateKeys[kStateFileSCL3]);
			}
			
			ImGui::SameLine(); 
			
			if (ImGui::Button("Open KBM File##3"))
			{
				requestStateFile(kStateKeys[kStateFileKBM3]);
			}
			
			ImGui::PushFont(lektonRegularFont);
			ImGui::PushItemWidth(-1);
			ImGui::LabelText("##scale_3_scl", fFileBaseName[kStateFileSCL3]);
			ImGui::LabelText("##scale_3_kbm", fFileBaseName[kStateFileKBM3]);
            ImGui::PopItemWidth();
            ImGui::PopFont();
            
            ImGui::EndChild(); // scale three pane
            

            
            ImGui::EndChild(); // left pane
            
            ImGui::SameLine(); 
            
            ImGui::BeginChild("right pane", ImVec2(UI_COLUMN_WIDTH, 0));
            
            
            //---
            
            ImGui::BeginChild("scale two pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 8.0f), true);
            
            ImGui::LabelText("##scale_2_label", "SCALE 2");
            
			if (ImGui::Button("Open SCL File##2"))
			{
				requestStateFile(kStateKeys[kStateFileSCL2]);
			}
			
			ImGui::SameLine(); 
			
			if (ImGui::Button("Open KBM File##2"))
			{
				requestStateFile(kStateKeys[kStateFileKBM2]);
			}
			
			ImGui::PushFont(lektonRegularFont);
			ImGui::PushItemWidth(-1);
			ImGui::LabelText("##scale_2_scl", fFileBaseName[kStateFileSCL2]);
			ImGui::LabelText("##scale_2_kbm", fFileBaseName[kStateFileKBM2]);
            ImGui::PopItemWidth();
            ImGui::PopFont();
            
            ImGui::EndChild(); // scale two pane
            
            //---
            
            ImGui::BeginChild("scale four pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 8.0f), true);
            
            ImGui::LabelText("##scale_4_label", "SCALE 4");
            
			if (ImGui::Button("Open SCL File##4"))
			{
				requestStateFile(kStateKeys[kStateFileSCL4]);
			}
			
			ImGui::SameLine(); 
			
			if (ImGui::Button("Open KBM File##4"))
			{
				requestStateFile(kStateKeys[kStateFileKBM4]);
			}
			
			ImGui::PushFont(lektonRegularFont);
			ImGui::PushItemWidth(-1);
			ImGui::LabelText("##scale_4_scl", fFileBaseName[kStateFileSCL4]);
			ImGui::LabelText("##scale_4_kbm", fFileBaseName[kStateFileKBM4]);
            ImGui::PopItemWidth();
            ImGui::PopFont();
            
            ImGui::EndChild(); // scale four pane
            
            ImGui::EndChild(); // right pane
            
            
            ImGui::EndChild(); // top pane
            
            //----------------------------------
            
            ImGui::BeginChild("sequence pane", ImVec2(0, 100 * scale_factor));
            
            ImGui::LabelText("##scale_4_label", "SEQUENCE");
            
            ImGui::PushFont(brunoAceStepFont);
            
            const std::string scale_choice[5] = { "0", "1", "2", "3", "4"};
            ImVec2 step_button_sz(32 * scale_factor,32 * scale_factor);
            
            // Step 1
            std::string step_1_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep1])];
            step_1_text.append("##step_1");
            const char* step_1_lbl = step_1_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.0625f)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
            
			if (ImGui::Button(step_1_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep1, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep1]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep1] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep1, fParameters[kParameterStep1]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.0625f)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep1, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 2
            std::string step_2_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep2])];
            step_2_text.append("##step_2");
            const char* step_2_lbl = step_2_text.c_str();
           
            if (fParameters[kParameterCurrentStep] == 0.125f)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
            
			if (ImGui::Button(step_2_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep2, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep2]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep2] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep2, fParameters[kParameterStep2]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.125f)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep2, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 3
            std::string step_3_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep3])];
            step_3_text.append("##step_3");
            const char* step_3_lbl = step_3_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.1875)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_3_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep3, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep3]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep3] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep3, fParameters[kParameterStep3]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.1875)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep3, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 4
            std::string step_4_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep4])];
            step_4_text.append("##step_4");
            const char* step_4_lbl = step_4_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.25)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_4_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep4, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep4]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep4] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep4, fParameters[kParameterStep4]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.25)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep4, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 5
            std::string step_5_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep5])];
            step_5_text.append("##step_5");
            const char* step_5_lbl = step_5_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.3125)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_5_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep5, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep5]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep5] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep5, fParameters[kParameterStep5]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.3125)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep5, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 6
            std::string step_6_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep6])];
            step_6_text.append("##step_6");
            const char* step_6_lbl = step_6_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.375)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_6_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep6, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep6]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep6] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep6, fParameters[kParameterStep6]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.375)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep6, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 7
            std::string step_7_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep7])];
            step_7_text.append("##step_7");
            const char* step_7_lbl = step_7_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.4375)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_7_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep7, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep7]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep7] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep7, fParameters[kParameterStep7]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.4375)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep7, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 8
            std::string step_8_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep8])];
            step_8_text.append("##step_8");
            const char* step_8_lbl = step_8_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.5)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_8_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep8, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep8]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep8] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep8, fParameters[kParameterStep8]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.5)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep8, false);
            }
			
			ImGui::SameLine(); 
			
			// Step 9
			std::string step_9_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep9])];
            step_9_text.append("##step_9");
            const char* step_9_lbl = step_9_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.5625)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_9_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep9, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep9]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep9] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep9, fParameters[kParameterStep9]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.5625)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep9, false);
            }
			
			ImGui::SameLine(); 
            
            // Step 10
            std::string step_10_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep10])];
            step_10_text.append("##step_10");
            const char* step_10_lbl = step_10_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.625)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_10_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep10, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep10]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep10] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep10, fParameters[kParameterStep10]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.625)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep10, false);
            }
			
			ImGui::SameLine();
			
			// Step 11
			std::string step_11_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep11])];
            step_11_text.append("##step_11");
            const char* step_11_lbl = step_11_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.6875)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_11_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep11, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep11]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep11] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep11, fParameters[kParameterStep11]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.6875)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep11, false);
            }
			
			ImGui::SameLine();
			
			// Step 12
			std::string step_12_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep12])];
            step_12_text.append("##step_12");
            const char* step_12_lbl = step_12_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.75)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_12_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep12, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep12]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep12] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep12, fParameters[kParameterStep12]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.75)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep12, false);
            }
			
			ImGui::SameLine();
			
			// Step 13
			std::string step_13_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep13])];
            step_13_text.append("##step_13");
            const char* step_13_lbl = step_13_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.8125)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_13_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep13, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep13]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep13] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep13, fParameters[kParameterStep13]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.8125)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep13, false);
            }
			
			ImGui::SameLine();
			
			// Step 14
			std::string step_14_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep14])];
            step_14_text.append("##step_14");
            const char* step_14_lbl = step_14_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.875)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_14_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep14, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep14]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep14] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep14, fParameters[kParameterStep14]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.875)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep14, false);
            }
			
			ImGui::SameLine();
			
			// Step 15
			std::string step_15_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep15])];
            step_15_text.append("##step_15");
            const char* step_15_lbl = step_15_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 0.9375)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_15_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep15, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep15]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep15] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep15, fParameters[kParameterStep15]);
			}
			
			if (fParameters[kParameterCurrentStep] == 0.9375)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep15, false);
            }
			
			ImGui::SameLine();
			
			// Step 16
			std::string step_16_text = scale_choice[static_cast<uint32_t>(fParameters[kParameterStep16])];
            step_16_text.append("##step_16");
            const char* step_16_lbl = step_16_text.c_str();
            
            if (fParameters[kParameterCurrentStep] == 1.000)
            {
				ImGui::PushStyleColor(ImGuiCol_Button, step_highlight_color);
			}
			
			if (ImGui::Button(step_16_lbl, step_button_sz))
			{
				if (ImGui::IsItemActivated())
                        editParameter(kParameterStep16, true);  
                        
				uint32_t cur_val = static_cast<uint32_t>(fParameters[kParameterStep16]);
                cur_val += 1;
				if (cur_val > 4)
					cur_val = 1;
				fParameters[kParameterStep16] = static_cast<float>(cur_val);
                setParameterValue(kParameterStep16, fParameters[kParameterStep16]);
			}
			
			if (fParameters[kParameterCurrentStep] == 1.000)
            {
				ImGui::PopStyleColor();
			}
			
			if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterStep16, false);
            }
			
			ImGui::PopFont();
			
			ImGui::EndChild(); // sequence pane
			
			ImGui::BeginChild("bottom pane", ImVec2(0, 0)); // bottom pane holds two colums
			
			ImGui::BeginChild("bottom left pane", ImVec2(UI_COLUMN_WIDTH, 0));
			
			// Multiplier
            if (ImGui::SliderInt("Step Multi", &ui_multiplier, static_cast<int>(controlLimits[kParameterMultiplier].first), static_cast<int>(controlLimits[kParameterMultiplier].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(kParameterMultiplier, true);
                
                fParameters[kParameterMultiplier] = static_cast<float>(ui_multiplier);
                setParameterValue(kParameterMultiplier, fParameters[kParameterMultiplier]);
            }
			
			 if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterMultiplier, false);
            }
            
            // Scale Glide
            if (ImGui::SliderFloat("Glide", &fParameters[kParameterScaleGlide], controlLimits[kParameterScaleGlide].first, controlLimits[kParameterScaleGlide].second, "%.2f", ImGuiSliderFlags_Logarithmic|ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(kParameterScaleGlide, true);

                setParameterValue(kParameterScaleGlide, fParameters[kParameterScaleGlide]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterScaleGlide, false);
            }
            
            // Loop Point
            if (ImGui::SliderInt("Loop Point", &ui_loopPoint, static_cast<int>(controlLimits[kParameterLoopPoint].first), static_cast<int>(controlLimits[kParameterLoopPoint].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(kParameterLoopPoint, true);
                
                fParameters[kParameterLoopPoint] = static_cast<float>(ui_loopPoint);
                setParameterValue(kParameterLoopPoint, fParameters[kParameterLoopPoint]);
            }
			
			 if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterLoopPoint, false);
            }
            
			ImGui::EndChild(); // bottom left pane
			
			ImGui::SameLine();
			
			ImGui::BeginChild("bottom right pane", ImVec2(UI_COLUMN_WIDTH, 0));
			
			// Measure
            const char* measure_types[3] = { "Beats", "Bars", "MIDI Note"};
            const char* current_measure_type = measure_types[static_cast<int32_t>(fParameters[kParameterMeasure])];

            ImGuiStyle& measure_style = ImGui::GetStyle();
            float measure_w = ImGui::CalcItemWidth();
            float measure_spacing = measure_style.ItemInnerSpacing.x;
            float measure_button_sz = ImGui::GetFrameHeight();
            ImGui::PushItemWidth(measure_w - measure_spacing * 2.0f - measure_button_sz * 2.0f);
            if (ImGui::BeginCombo("##measure_combo", current_measure_type, ImGuiComboFlags_NoArrowButton))
            {
                if (ImGui::IsItemActivated())
                        editParameter(kParameterMeasure, true);
                        
                for (int n = 0; n < IM_ARRAYSIZE(measure_types); n++)
                {
                    bool is_selected = (current_measure_type == measure_types[n]);
                    if (ImGui::Selectable(measure_types[n], is_selected))
                    {
                        current_measure_type = measure_types[n];
                        fParameters[kParameterMeasure] = static_cast<float>(n);
                        setParameterValue(kParameterMeasure, fParameters[kParameterMeasure]);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterMeasure, false);
            }
            
            ImGui::PopItemWidth();
            
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,220)); // white arrows
            
            ImGui::SameLine(0, measure_spacing);
            if (ImGui::ArrowButton("##measure_l", ImGuiDir_Left))
            {
                if (ImGui::IsItemActivated())
                        editParameter(kParameterMeasure, true);  
                                      
                int32_t current_measure_index = static_cast<int32_t>(fParameters[kParameterMeasure]);
                if (current_measure_index > 0)
                {
                    current_measure_index -= 1;
                    current_measure_type = measure_types[current_measure_index];
                    fParameters[kParameterMeasure] = static_cast<float>(current_measure_index);
                    setParameterValue(kParameterMeasure, fParameters[kParameterMeasure]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterMeasure, false);
            }
            
            ImGui::SameLine(0, measure_spacing);
            if (ImGui::ArrowButton("##measure_r", ImGuiDir_Right))
            {
                if (ImGui::IsItemActivated())
                        editParameter(kParameterMeasure, true);
                        
                int32_t current_measure_index = static_cast<int32_t>(fParameters[kParameterMeasure]);
                if (current_measure_index < 2)
                {
                    current_measure_index += 1;
                    current_measure_type = measure_types[current_measure_index];
                    fParameters[kParameterMeasure] = static_cast<float>(current_measure_index);
                    setParameterValue(kParameterMeasure, fParameters[kParameterMeasure]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterMeasure, false);
            }
            
            ImGui::PopStyleColor(); // undo white text for arrows
            
            ImGui::SameLine(0, measure_style.ItemInnerSpacing.x);
            ImGui::Text("Step Type");
            
            // Offset
            if (ImGui::SliderFloat("Offset", &fParameters[kParameterOffset], controlLimits[kParameterOffset].first, controlLimits[kParameterOffset].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(kParameterOffset, true);

                setParameterValue(kParameterOffset, fParameters[kParameterOffset]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(kParameterOffset, false);
            }
			
			ImGui::EndChild(); // bottom right pane
			
			ImGui::EndChild(); // bottom pane
			
			// Error popup
			if (show_error_popup)
				ImGui::OpenPopup("error_popup");
			if (ImGui::BeginPopup("error_popup"))
			{
				ImGui::PushFont(lektonRegularFont);
				ImGui::Text(errorText);
				ImGui::PopFont();
				show_error_popup = false;
				ImGui::EndPopup();
			}
                       
		}
		ImGui::End();
    }

    // -------------------------------------------------------------------------------------------------------

private:
    // Parameters
    float fParameters[kParameterCount];

    // State (files)
    String fState[kStateCount];
    String fFileBaseName[kStateCount];
    
    Tunings::Tuning utuning1, utuning2, utuning3, utuning4;
    
    // UI stuff
    double scale_factor;
    int UI_COLUMN_WIDTH;
    ResizeHandle fResizeHandle;
    ImFont* brunoAceFont;
    ImFont* brunoAceStepFont;
    ImFont* brunoAceSCFont;
    ImFont* lektonRegularFont;
    
    bool show_error_popup;
    String errorText;

    // int and bool variables required for Dear ImGui SliderInt and CheckBox widgets.
    int ui_multiplier;
	int ui_loopPoint;
    

    
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleSequenceUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ScaleSequenceUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
