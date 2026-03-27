#include "copyselectmenu.h"

#include "preset.h"
#include "system.h"
#include "eepr.h"

#include "display_task.h"
#include "sharc_task.h"

Preset::TSelectionMask CopySelectMenu::m_selectionMask =
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
	if(blinkFlag && m_copied)
	{
		topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
	}
}

void CopySelectMenu::encoderPressed()
{
	switch(element[m_elementNum].type)
	{
		case ActionOk:
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(38, 2, Font::fnt12x13, Font::fnsNormal, (uint8_t*)"Copy OK!");
			Preset::Copy(m_targetPresetNum, m_selectionMask);
			m_copied = 1;
			restartBlinking(0);
			break;
		}
		case ActionReturn:
		{
			topLevelMenu->returnFromChildMenu();
			break;
		}
		case ActionSelectAll:
		{
			kgp_sdk_libc::memset(&m_selectionMask, 1, sizeof(Preset::TSelectionMask));
			printPage();
			break;
		}
		case ActionDeselectAll:
		{
			kgp_sdk_libc::memset(&m_selectionMask, 0, sizeof(Preset::TSelectionMask));
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
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, (Font::TFontState)(highlight * Font::fnsHighlight), (uint8_t*)element.name);
			break;
		}
		case StringShort:
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, (Font::TFontState)(highlight * Font::fnsHighlight), (uint8_t*)element.name);
			DisplayTask->CheckBox(numOnPage%2 * 6 * 10 + 6 * 6, numOnPage / 2, *element.checked_ptr);
		break;
		}
		case StringLong:
		{
			DisplayTask->StringOut(0, numOnPage / 2, Font::fntSystem, (Font::TFontState)(highlight * Font::fnsHighlight), (uint8_t*)element.name);
			DisplayTask->CheckBox(12 * 8, numOnPage / 2, *element.checked_ptr);
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
