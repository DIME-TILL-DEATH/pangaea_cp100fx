#include "copyselectmenu.h"

#include "display.h"

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
}

void CopySelectMenu::show(TShowMode showMode)
{
	currentMenu = this;
	printPage();
}

void CopySelectMenu::task()
{

}

void CopySelectMenu::encoderPressed()
{

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
		case StringShort:
		{
			DisplayTask->StringOut(numOnPage%2 * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
//			DisplayTask->StringOut((numOnPage%2 + 1) * 6 * 10, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)"ch");
			break;
		}
		case StringLong:
		{
			DisplayTask->StringOut(0, numOnPage / 2, Font::fntSystem, 2 * highlight, (uint8_t*)element.name);
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
	while(elementOnPageNum < ElementsOnPage)
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
