#include "copyselectmenu.h"

#include "display.h"
#include "preset.h"
#include "eepr.h"

CopySelectMenu::TSelectionMask CopySelectMenu::m_selectionMask =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1
};

CopySelectMenu::CopySelectMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_COPY_SELECTION;

	element[0] = {ActionReturn, nullptr, "Return"};
	element[1] = {ActionOk, nullptr, "Ok"};

	element[2] = {ActionSelectAll, nullptr, "Sel.all"};
	element[3] = {ActionDeselectAll, nullptr, "Clr.all"};

	element[4] = {StringLong, &m_selectionMask.name, "Name"};
	element[5] = {StringLong, &m_selectionMask.comment, "Comment"};
	element[6] = {StringLong, &m_selectionMask.controllers, "Controllers"};

	element[7] = {StringShort, &m_selectionMask.rf, "RF"};
	element[8] = {StringShort, &m_selectionMask.gt, "GT"};
	element[9] = {StringShort, &m_selectionMask.cm, "CM"};
	element[10] = {StringShort, &m_selectionMask.pr, "PR"};
	element[11] = {StringShort, &m_selectionMask.pa, "PA"};
	element[12] = {StringShort, &m_selectionMask.ir, "IR"};
	element[13] = {StringShort, &m_selectionMask.eq, "EQ"};

	element[14] = {StringShort, &m_selectionMask.fl, "FL"};
	element[15] = {StringShort, &m_selectionMask.ph, "PH"};
	element[16] = {StringShort, &m_selectionMask.ch, "CH"};
	element[17] = {StringShort, &m_selectionMask.dl, "DL"};
	element[18] = {StringShort, &m_selectionMask.er, "ER"};
	element[19] = {StringShort, &m_selectionMask.rv, "RV"};
	element[20] = {StringShort, &m_selectionMask.tr, "TR"};
	element[21] = {StringShort, &m_selectionMask.pv, "PR.VOL"};
	element[22] = {StringShort, &m_selectionMask.att, "ATT"};
}

void CopySelectMenu::show(TShowMode showMode)
{
	currentMenu = this;
	m_copied = 0;
	printPage();
}

void CopySelectMenu::task()
{
	if(blinkFlag_fl && m_copied)
	{
		topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
	}
}

void CopySelectMenu::copyPreset(const TSelectionMask& selectionMask, uint8_t targetPresetNum)
{
	EEPROM_LoadPresetToBuffer(targetPresetNum, presetBuffer);

	if(selectionMask.name)
	{
		kgp_sdk_libc::memcpy(presetBuffer, currentPreset.name, 15);
	}

	if(selectionMask.comment)
	{
		kgp_sdk_libc::memcpy(presetBuffer + 15, currentPreset.comment, 15);
	}

	if(selectionMask.rf)
	{
			kgp_sdk_libc::memcpy(&presetBuffer[PRESET_DATA_OFFSET + RFILTER_MIX], &currentPreset.modules.rawData[RFILTER_MIX], sizeof(Preset::TRfData));
			presetBuffer[PRESET_DATA_OFFSET + ENABLE_RESONANCE_FILTER] = currentPreset.modules.rawData[ENABLE_RESONANCE_FILTER];
			presetBuffer[PRESET_DATA_OFFSET + RFILTER_LFO_TYPE] = currentPreset.modules.rawData[RFILTER_LFO_TYPE];
	}

	if(selectionMask.gt)
	{
		for(uint8_t i = 0; i<3; i++)
			presetBuffer[30+gate_thr+i] = currentPreset.modules.rawData[gate_thr+i];

		presetBuffer[PRESET_DATA_OFFSET + ENABLE_GATE] = currentPreset.modules.rawData[ENABLE_GATE];
	}

	if(selectionMask.cm)
	{
		for(uint8_t i = 0; i<5; i++)
			presetBuffer[30+comp_thr+i] = currentPreset.modules.rawData[comp_thr+i];
		presetBuffer[30+compr] = currentPreset.modules.rawData[compr];
	}

	if(selectionMask.pr)
	{
		for(uint8_t i = 0; i<6; i++)
			presetBuffer[30+pre_gain+i] = currentPreset.modules.rawData[pre_gain+i];
		presetBuffer[30+pream] = currentPreset.modules.rawData[pream];
	}

	if(selectionMask.pa)
	{
		for(uint8_t i = 0; i<3; i++)
			presetBuffer[30+am_v+i] = currentPreset.modules.rawData[am_v+i];
		presetBuffer[30+pre_v] = currentPreset.modules.rawData[pre_v];
		presetBuffer[30+amp] = currentPreset.modules.rawData[amp];
	}

	if(selectionMask.ir)
	{
		for(uint8_t i = 0; i<6; i++)
			presetBuffer[30+vol+i] = currentPreset.modules.rawData[vol+i];

		kgp_sdk_libc::memcpy(&presetBuffer[13344], cab1.name.string, 64);
		kgp_sdk_libc::memcpy(&presetBuffer[25696], cab2.name.string, 64);

		for(uint16_t i = 0; i<12288; i++)
			presetBuffer[1056+i] = cab1.data[i];

		if(cab_type==CAB_CONFIG_STEREO)
			for(uint16_t i = 0; i<4096*3; i++)
				presetBuffer[13408+i] = cab2.data[i];
		else
			for(uint16_t i = 0; i<4096*3; i++)
				presetBuffer[25760+i] = cab1.data[i + 4096 * 3];

		for(uint16_t i = 0; i<512; i++)
			presetBuffer[38048+i] = Preset::impulsePath[i];

		cab_data_ready = true; // Kostyl
		send_cab_data(0, targetPresetNum+1, 0);

		if(cab_type==CAB_CONFIG_STEREO)
			send_cab_data1(0, targetPresetNum+1);

		cab_data_ready = false;

		presetBuffer[30+cab] = currentPreset.modules.rawData[cab];
	}

	if(selectionMask.eq)
	{
		for(uint8_t i = 0; i<8; i++)
			presetBuffer[30+eq1+i] = currentPreset.modules.rawData[eq1+i];
		for(uint8_t i = 0; i<10; i++)
			presetBuffer[30+f1+i] = currentPreset.modules.rawData[f1+i];
		presetBuffer[30+eq] = currentPreset.modules.rawData[eq];
		presetBuffer[30+eq_pr_po] = currentPreset.modules.rawData[eq_pr_po];
	}

	if(selectionMask.ph)
	{
		for(uint8_t i = 0; i<6; i++)
			presetBuffer[30+phaser_vol+i] = currentPreset.modules.rawData[phaser_vol+i];
		presetBuffer[30+phas] = currentPreset.modules.rawData[phas];
		presetBuffer[30+hpf_ph] = currentPreset.modules.rawData[hpf_ph];
		presetBuffer[30+phas_pos] = currentPreset.modules.rawData[phas_pos];
	}

	if(selectionMask.fl)
	{
		for(uint8_t i = 0; i<6; i++)
			presetBuffer[30+fl_v+i] = currentPreset.modules.rawData[fl_v+i];
		presetBuffer[30+fl] = currentPreset.modules.rawData[fl];
		presetBuffer[30+hpf_fl] = currentPreset.modules.rawData[hpf_fl];
		presetBuffer[30+flan_pos] = currentPreset.modules.rawData[flan_pos];
	}

	if(selectionMask.ch)
	{
		for(uint8_t i = 0; i<5; i++)
			presetBuffer[30+chor_volum+i] = currentPreset.modules.rawData[chor_volum+i];
		presetBuffer[30+chor] = currentPreset.modules.rawData[chor];
		presetBuffer[30+hpf_ch] = currentPreset.modules.rawData[hpf_ch];
	}

	if(selectionMask.dl)
	{
		for(uint8_t i = 0; i<11; i++)
			presetBuffer[30+d_vol+i] = currentPreset.modules.rawData[d_vol+i];

		presetBuffer[1054] = delay_time;
		presetBuffer[1055] = delay_time>>8;

		presetBuffer[30+delay] = currentPreset.modules.rawData[delay];
		presetBuffer[30+d_tap_t] = currentPreset.modules.rawData[d_tap_t];
		presetBuffer[30+d_tail] = currentPreset.modules.rawData[d_tail];
	}

	if(selectionMask.er)
	{
		for(uint8_t i = 0; i<2; i++)
			presetBuffer[30+early_vol+i] = currentPreset.modules.rawData[early_vol+i];
		presetBuffer[30+early] = currentPreset.modules.rawData[early];
	}

	if(selectionMask.rv)
	{
		for(uint8_t i = 0; i<7; i++)
			presetBuffer[30+r_vol+i] = currentPreset.modules.rawData[r_vol+i];
		presetBuffer[30+reve] = currentPreset.modules.rawData[reve];
		presetBuffer[30+rev_t] = currentPreset.modules.rawData[rev_t];
		presetBuffer[30+rev_di] = currentPreset.modules.rawData[rev_di];
		presetBuffer[30+r_pre] = currentPreset.modules.rawData[r_pre];
		presetBuffer[30+r_tail] = currentPreset.modules.rawData[r_tail];
	}

	if(selectionMask.tr)
	{
		for(uint8_t i = 0; i<4; i++)
			presetBuffer[30+tr_vol+i] = currentPreset.modules.rawData[tr_vol+i];

		presetBuffer[30+trem] = currentPreset.modules.rawData[trem];
		presetBuffer[30+t_tap_t] = currentPreset.modules.rawData[t_tap_t];
		presetBuffer[30+tr_lfo_t] = currentPreset.modules.rawData[tr_lfo_t];
	}

	if(selectionMask.pv)
	{
		presetBuffer[30 + pres_lev] = currentPreset.modules.paramData.preset_volume;
	}

	if(selectionMask.att)
	{
		presetBuffer[PRESET_DATA_OFFSET + preset_att] = currentPreset.modules.paramData.attenuator;
	}

	if(selectionMask.controllers)
	{
		for(uint16_t i = 0 ; i < 8 ; i += 4)
		{
			presetBuffer[542 + i] = currentPreset.controller[i].src;
			presetBuffer[542 + i + 1] = currentPreset.controller[i].dst;
			presetBuffer[542 + i + 2] = currentPreset.controller[i].minVal;
			presetBuffer[542 + i + 3] = currentPreset.controller[i].maxVal;
		}
	}

	write_prog_temp(targetPresetNum);
	cab_data_ready = true; // Kostyl
	send_cab_data(1, targetPresetNum+1, 1);
	cab_data_ready = false;

	Preset::Change();
}

void CopySelectMenu::encoderPressed()
{
	switch(element[m_elementNum].type)
	{
		case ActionOk:
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(38, 2, Font::fnt12x13, 0, (uint8_t*)"Copy OK!");
			copyPreset(m_selectionMask, m_targetPresetNum);
			m_copied = 1;
			tim5_start(0);
			break;
		}
		case ActionReturn:
		{
			topLevelMenu->returnFromChildMenu();
			break;
		}
		case ActionSelectAll:
		{
			kgp_sdk_libc::memset(&m_selectionMask, 1, sizeof(TSelectionMask));
			printPage();
			break;
		}
		case ActionDeselectAll:
		{
			kgp_sdk_libc::memset(&m_selectionMask, 0, sizeof(TSelectionMask));
			printPage();
			break;
		}
		case StringShort:
		case StringLong:
		{
			*element[m_elementNum].checked_ptr = *element[m_elementNum].checked_ptr ? 0 : 1;
			printPage();
			break;
		}

	}
}

void CopySelectMenu::encoderCounterClockwise()
{
	if(m_elementNum > 0) m_elementNum--;

	printPage();
}

void CopySelectMenu::encoderClockwise()
{
	if(m_elementNum < ElementsCount - 1) m_elementNum++;

	printPage();
}

void CopySelectMenu::printElement(const SelectionElement& element, uint8_t numOnPage, bool highlight)
{
	switch(element.type)
	{
		case ActionOk:
		case ActionReturn:
		case ActionSelectAll:
		case ActionDeselectAll:
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			break;
		}
		case StringShort:
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			DisplayTask->CheckBox(numOnPage%2 * 6 * 10 + 6 * 6, numOnPage / 2, *element.checked_ptr);
//			DisplayTask->StringOut((numOnPage%2 + 1) * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)"ch");
			break;
		}
		case StringLong:
		{
			DisplayTask->StringOut(0, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			DisplayTask->CheckBox(12 * 8, numOnPage / 2, *element.checked_ptr);
//			DisplayTask->StringOut(6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)"ch");
			break;
		}

	}
}

void CopySelectMenu::printPage()
{
	DisplayTask->Clear();

	uint8_t prevElementRowsCount = 0;
	for(uint8_t i=0; i < m_elementNum; i++)
	{
		prevElementRowsCount += elementIncrementIndex(element[i]);
	}

	uint8_t currentPageNumber = prevElementRowsCount / ElementsOnPage;

	uint8_t startElementNumber = 0, i = 0;
	while(i < currentPageNumber * ElementsOnPage - 1)
	{
		i += elementIncrementIndex(element[i]);
		startElementNumber++;
	}

	i = 0;
	uint8_t elementOnPageNum = 0;
	while(elementOnPageNum < std::min((int)ElementsOnPage, ElementsCount - startElementNumber))
	{
		bool highlighElement = false;
		if(m_elementNum == startElementNumber + i) highlighElement = true;
		printElement(element[startElementNumber + i], elementOnPageNum, highlighElement);

		elementOnPageNum += elementIncrementIndex(element[startElementNumber + i]);
		i++;
	}
}

uint8_t CopySelectMenu::elementIncrementIndex(const SelectionElement& element)
{
	switch(element.type)
	{
		case ActionOk: return 1;
		case ActionReturn: return 1;
		case ActionSelectAll: return 1;
		case ActionDeselectAll: return 1;
		case StringLong: return 2;
		case StringShort: return 1;
		default: return 1;
	}
}
