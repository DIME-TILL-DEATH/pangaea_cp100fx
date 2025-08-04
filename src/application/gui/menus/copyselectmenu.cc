#include "copyselectmenu.h"

#include "display.h"
#include "preset.h"
#include "eepr.h"

CopySelectMenu::CopySelectMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_COPY_SELECTION;

	element[0] = {ActionReturn, Checked, nullptr, "Return"};
	element[1] = {ActionOk, Checked, nullptr, "Ok"};

	element[2] = {StringLong, Checked, &m_selectionMask.name, "Name"};
	element[3] = {StringLong, Checked, &m_selectionMask.comment, "Comment"};
	element[4] = {StringLong, Checked, &m_selectionMask.controllers, "Controllers"};

	element[5] = {StringShort, Checked, &m_selectionMask.rf, "RF"};
	element[6] = {StringShort, Checked, &m_selectionMask.gt, "GT"};
	element[7] = {StringShort, Checked, &m_selectionMask.cm, "CM"};
	element[8] = {StringShort, Checked, &m_selectionMask.pr, "PR"};
	element[9] = {StringShort, Checked, &m_selectionMask.pa, "PA"};
	element[10] = {StringShort, Checked, &m_selectionMask.ir, "IR"};
	element[11] = {StringShort, Checked, &m_selectionMask.eq, "EQ"};

	element[12] = {StringShort, Checked, &m_selectionMask.fl, "FL"};
	element[13] = {StringShort, Checked, &m_selectionMask.ph, "PH"};
	element[14] = {StringShort, Checked, &m_selectionMask.ch, "CH"};
	element[15] = {StringShort, Checked, &m_selectionMask.dl, "DL"};
	element[16] = {StringShort, Checked, &m_selectionMask.eq, "ER"};
	element[17] = {StringShort, Checked, &m_selectionMask.rv, "RV"};
	element[18] = {StringShort, Checked, &m_selectionMask.tr, "TR"};
	element[19] = {StringShort, Checked, &m_selectionMask.pv, "PR.VOL"};

	kgp_sdk_libc::memset(&m_selectionMask, 1, sizeof(TSelectionMask));
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
		topLevelMenu->returnFromChildMenu();
	}
}

void CopySelectMenu::copyPreset(const TSelectionMask& selectionMask, uint8_t targetPresetNum)
{
	if(selectionMask.name)
	{
		kgp_sdk_libc::memcpy(preset_temp, currentPreset.name, 15);
	}

	if(selectionMask.comment)
	{
		kgp_sdk_libc::memcpy(preset_temp + 15, currentPreset.comment, 15);
	}

	if(selectionMask.rf)
	{
			for(uint8_t i = 0; i<11; i++)
				preset_temp[30+RFILTER_MIX+i] = currentPreset.modules.rawData[RFILTER_MIX+i];

			preset_temp[30+moog] = currentPreset.modules.rawData[moog];
			preset_temp[30+mog_gen_t] = currentPreset.modules.rawData[mog_gen_t];
	}

	if(selectionMask.gt)
	{
		for(uint8_t i = 0; i<3; i++)
			preset_temp[30+gate_thr+i] = currentPreset.modules.rawData[gate_thr+i];

		preset_temp[30+gate] = currentPreset.modules.rawData[gate];
	}

	if(selectionMask.cm)
	{
		for(uint8_t i = 0; i<5; i++)
			preset_temp[30+comp_thr+i] = currentPreset.modules.rawData[comp_thr+i];
		preset_temp[30+compr] = currentPreset.modules.rawData[compr];
	}

	if(selectionMask.pr)
	{
		for(uint8_t i = 0; i<6; i++)
			preset_temp[30+pre_gain+i] = currentPreset.modules.rawData[pre_gain+i];
		preset_temp[30+pream] = currentPreset.modules.rawData[pream];
	}

	if(selectionMask.pa)
	{
		for(uint8_t i = 0; i<3; i++)
			preset_temp[30+am_v+i] = currentPreset.modules.rawData[am_v+i];
		preset_temp[30+pre_v] = currentPreset.modules.rawData[pre_v];
		preset_temp[30+amp] = currentPreset.modules.rawData[amp];
	}

	if(selectionMask.ir)
	{
		for(uint8_t i = 0; i<6; i++)
			preset_temp[30+vol+i] = currentPreset.modules.rawData[vol+i];
		for(uint8_t i = 0; i<64; i++)
			preset_temp[13344+i] = cab1.name.string[i];
		for(uint8_t i = 0; i<64; i++)
			preset_temp[25696+i] = cab2.name.string[i];
		for(uint16_t i = 0; i<12288; i++)
			preset_temp[1056+i] = cab1.data[i];
		if(cab_type==CAB_CONFIG_STEREO)
			for(uint16_t i = 0; i<12288; i++)
				preset_temp[13408+i] = cab2.data[i];
		else
			for(uint16_t i = 0; i < 4096 * 3; i++)
				preset_temp[25760+i] = cab1.data[i + 4096 * 3];
		for(uint16_t i = 0; i<512; i++)
			preset_temp[38048+i] = Preset::impulsePath[i];

		send_cab_data(0, targetPresetNum+1, 0);

		if(cab_type==2)
			send_cab_data1(0, targetPresetNum+1);

		preset_temp[30+cab] = currentPreset.modules.rawData[cab];
	}

	if(selectionMask.eq)
	{
		for(uint8_t i = 0; i<8; i++)
			preset_temp[30+eq1+i] = currentPreset.modules.rawData[eq1+i];
		for(uint8_t i = 0; i<10; i++)
			preset_temp[30+f1+i] = currentPreset.modules.rawData[f1+i];
		preset_temp[30+eq] = currentPreset.modules.rawData[eq];
		preset_temp[30+eq_pr_po] = currentPreset.modules.rawData[eq_pr_po];
	}

	if(selectionMask.ph)
	{
		for(uint8_t i = 0; i<6; i++)
			preset_temp[30+phaser_vol+i] = currentPreset.modules.rawData[phaser_vol+i];
		preset_temp[30+phas] = currentPreset.modules.rawData[phas];
		preset_temp[30+hpf_ph] = currentPreset.modules.rawData[hpf_ph];
		preset_temp[30+phas_pos] = currentPreset.modules.rawData[phas_pos];
	}

	if(selectionMask.fl)
	{
		for(uint8_t i = 0; i<6; i++)
			preset_temp[30+fl_v+i] = currentPreset.modules.rawData[fl_v+i];
		preset_temp[30+fl] = currentPreset.modules.rawData[fl];
		preset_temp[30+hpf_fl] = currentPreset.modules.rawData[hpf_fl];
		preset_temp[30+flan_pos] = currentPreset.modules.rawData[flan_pos];
	}

	if(selectionMask.ch)
	{
		for(uint8_t i = 0; i<5; i++)
			preset_temp[30+chor_volum+i] = currentPreset.modules.rawData[chor_volum+i];
		preset_temp[30+chor] = currentPreset.modules.rawData[chor];
		preset_temp[30+hpf_ch] = currentPreset.modules.rawData[hpf_ch];
	}

	if(selectionMask.dl)
	{
		for(uint8_t i = 0; i<11; i++)
			preset_temp[30+d_vol+i] = currentPreset.modules.rawData[d_vol+i];
		preset_temp[1054] = delay_time;
		preset_temp[1055] = delay_time>>8;
		preset_temp[30+delay] = currentPreset.modules.rawData[delay];
		preset_temp[30+d_tap_t] = currentPreset.modules.rawData[d_tap_t];
		preset_temp[30+d_tail] = currentPreset.modules.rawData[d_tail];
	}

	if(selectionMask.er)
	{
		for(uint8_t i = 0; i<2; i++)
			preset_temp[30+early_vol+i] = currentPreset.modules.rawData[early_vol+i];
		preset_temp[30+early] = currentPreset.modules.rawData[early];
	}

	if(selectionMask.rv)
	{
		for(uint8_t i = 0; i<7; i++)
			preset_temp[30+r_vol+i] = currentPreset.modules.rawData[r_vol+i];
		preset_temp[30+reve] = currentPreset.modules.rawData[reve];
		preset_temp[30+rev_t] = currentPreset.modules.rawData[rev_t];
		preset_temp[30+rev_di] = currentPreset.modules.rawData[rev_di];
		preset_temp[30+r_pre] = currentPreset.modules.rawData[r_pre];
		preset_temp[30+r_tail] = currentPreset.modules.rawData[r_tail];
	}

	if(selectionMask.tr)
	{
		for(uint8_t i = 0; i<4; i++)
			preset_temp[30+tr_vol+i] = currentPreset.modules.rawData[tr_vol+i];

		preset_temp[30+trem] = currentPreset.modules.rawData[trem];
		preset_temp[30+t_tap_t] = currentPreset.modules.rawData[t_tap_t];
		preset_temp[30+tr_lfo_t] = currentPreset.modules.rawData[tr_lfo_t];
	}

	if(selectionMask.pv)
	{
		preset_temp[30 + pres_lev] = currentPreset.modules.paramData.preset_volume;
	}

	if(selectionMask.controllers)
	{
		for(uint16_t i = 0 ; i < 8 ; i += 4)
		{
			preset_temp[542 + i] = currentPreset.controller[i].src;
			preset_temp[542 + i + 1] = currentPreset.controller[i].dst;
			preset_temp[542 + i + 2] = currentPreset.controller[i].minVal;
			preset_temp[542 + i + 3] = currentPreset.controller[i].maxVal;
		}
	}

	send_cab_data(1, targetPresetNum+1, 1);
	write_prog_temp(targetPresetNum);
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
		case StringShort:
		case StringLong:
		{
			element[m_elementNum].state = element[m_elementNum].state ? ElementState::Unchecked : ElementState::Checked;
			*element[m_elementNum].value_ptr = element[m_elementNum].state;
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
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			break;
		}
		case StringShort:
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			DisplayTask->CheckBox(numOnPage%2 * 6 * 10 + 6 * 6, numOnPage / 2, element.state);
//			DisplayTask->StringOut((numOnPage%2 + 1) * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)"ch");
			break;
		}
		case StringLong:
		{
			DisplayTask->StringOut(0, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
			DisplayTask->CheckBox(12 * 8, numOnPage / 2, element.state);
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
	while(elementOnPageNum < std::min((int)ElementsOnPage, 20 - startElementNumber))
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
		case StringLong: return 2;
		case StringShort: return 1;
	}
}
