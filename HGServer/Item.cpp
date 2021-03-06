// Item.cpp: implementation of the CItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Item.h"

extern void PutLogList(char* cMsg);
extern char G_cTxt[512];
extern char	G_cData50000[50000];
extern void PutLogFileList(char* cStr);
extern void PutAdminLogFileList(char* cStr);
extern void PutItemLogFileList(char* cStr);
extern void PutLogEventFileList(char* cStr);
extern void PutHackLogFileList(char* cStr);
extern void PutPvPLogFileList(char* cStr);
extern FILE* pLogFile;
extern HWND	G_hWnd;

#pragma warning (disable : 4996 6011 6001 4244 4018 6385 6386 26451 6054 4267 6053 6031)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItem::CItem()
{
	ZeroMemory(m_cName, sizeof(m_cName));
	m_sSprite = 0;
	m_sSpriteFrame = 0;
											  
	m_sItemEffectValue1 = 0;
	m_sItemEffectValue2 = 0;
	m_sItemEffectValue3 = 0; 

	m_sItemEffectValue4 = 0;
	m_sItemEffectValue5 = 0;
	m_sItemEffectValue6 = 0; 

	m_dwCount = 1;
	m_sTouchEffectType   = 0;
	m_sTouchEffectValue1 = 0;
	m_sTouchEffectValue2 = 0;
	m_sTouchEffectValue3 = 0;
	
	m_cItemColor = 0;
	m_sItemSpecEffectValue1 = 0;
	m_sItemSpecEffectValue2 = 0;
	m_sItemSpecEffectValue3 = 0;

	m_sSpecialEffectValue1 = 0;
	m_sSpecialEffectValue2 = 0;
	
	m_wCurLifeSpan = 0;
	m_dwAttribute   = 0;

	m_cCategory = NULL;
	m_sIDnum    = 0;

	m_bIsForSale = FALSE;
}

CItem::~CItem()
{

}

void CGame::ItemDepleteHandler(int iClientH, short sItemIndex, BOOL bIsUseItemResult, BOOL bIsItemUsed)
{
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
	if ((bIsItemUsed == 1) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_MATERIAL) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 380) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 381) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 382)) 
	{
		_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, -1, m_pClientList[iClientH]->m_pItemList[sItemIndex], FALSE);
	}
	else if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 247) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 248)) 
	{
		_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, -1, m_pClientList[iClientH]->m_pItemList[sItemIndex], FALSE);
	}

	ReleaseItemHandler(iClientH, sItemIndex, TRUE);
	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMDEPLETED_ERASEITEM, sItemIndex, (int)bIsUseItemResult, NULL, NULL);
	delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
	m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;
	m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	iCalcTotalWeight(iClientH);
}

void CGame::CalcTotalItemEffect(int iClientH, int iEquipItemID, BOOL bNotify)
{
	short sItemIndex;
	int  i, iPrevSAType, iTemp;
	char cEquipPos;
	double dV1, dV2, dV3;
	DWORD  dwSWEType, dwSWEValue;
	short  sTemp;
	int iShieldHPrec, iShieldSPrec, iShieldMPrec, iTotalAngelicstatas;

	iTotalAngelicstatas = m_pClientList[iClientH]->m_iAngelicStr + (16 * m_pClientList[iClientH]->m_iAngelicInt) + (256 * m_pClientList[iClientH]->m_iAngelicDex) + (16 * 256 * m_pClientList[iClientH]->m_iAngelicMag);

	iShieldHPrec = iShieldSPrec = iShieldMPrec = 0;

	if (m_pClientList[iClientH] == NULL) return;

	if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) &&
		(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)) {

		// 잘못된 무기 장착 조합이다. 둘 중 하나를 내려 놓는다. 
		if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] != NULL) {
			// 한손 검의 장착 상태를 해제한다. 
			m_pClientList[iClientH]->m_bIsItemEquipped[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] = FALSE;
			m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] = -1;
		}
	}

	m_pClientList[iClientH]->m_iAngelicStr = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicInt = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicDex = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicMag = 0; // By Snoopy81 
	SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 0, 0);

	if (m_pClientList[iClientH]->m_iWantedLevel > 0) SetWantedFlag(iClientH, DEF_OWNERTYPE_PLAYER, 1); // Wanted System

	m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
	m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
	m_pClientList[iClientH]->m_cAttackBonus_SM = 0;

	m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
	m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
	m_pClientList[iClientH]->m_cAttackBonus_L = 0;

	m_pClientList[iClientH]->m_iHitRatio = 0;

	m_pClientList[iClientH]->m_iDefenseRatio = (m_pClientList[iClientH]->m_iDex * 2);

	m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 0;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[i] = 0;

	m_pClientList[iClientH]->m_iManaSaveRatio = 0;
	m_pClientList[iClientH]->m_iAddResistMagic = 0;

	m_pClientList[iClientH]->m_iAddPhysicalDamage = 0;
	m_pClientList[iClientH]->m_iAddMagicalDamage = 0;

	m_pClientList[iClientH]->m_bIsLuckyEffect = FALSE;
	m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = -1;
	m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = 0;

	m_pClientList[iClientH]->m_iAddAbsAir = 0;	// 속성별 대미지 흡수
	m_pClientList[iClientH]->m_iAddAbsEarth = 0;
	m_pClientList[iClientH]->m_iAddAbsFire = 0;
	m_pClientList[iClientH]->m_iAddAbsWater = 0;

	m_pClientList[iClientH]->m_iCustomItemValue_Attack = 0;
	m_pClientList[iClientH]->m_iCustomItemValue_Defense = 0;

	m_pClientList[iClientH]->m_iMinAP_SM = 0;
	m_pClientList[iClientH]->m_iMinAP_L = 0;

	m_pClientList[iClientH]->m_iMaxAP_SM = 0;
	m_pClientList[iClientH]->m_iMaxAP_L = 0;

	m_pClientList[iClientH]->m_iSpecialWeaponEffectType = 0;	// 희귀 아이템 효과 종류: 0-None 1-필살기대미지추가 2-중독효과 3-정의의 4-저주의
	m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = 0;	// 희귀 아이템 효과 값

	m_pClientList[iClientH]->m_iAddHP = m_pClientList[iClientH]->m_iAddSP = m_pClientList[iClientH]->m_iAddMP = 0;
	m_pClientList[iClientH]->m_iAddAR = m_pClientList[iClientH]->m_iAddPR = m_pClientList[iClientH]->m_iAddDR = 0;
	m_pClientList[iClientH]->m_iAddMR = m_pClientList[iClientH]->m_iAddAbsPD = m_pClientList[iClientH]->m_iAddAbsMD = 0;
	m_pClientList[iClientH]->m_iAddCD = m_pClientList[iClientH]->m_iAddExp = m_pClientList[iClientH]->m_iAddGold = 0;

	iPrevSAType = m_pClientList[iClientH]->m_iSpecialAbilityType;

	m_pClientList[iClientH]->m_iSpecialAbilityType = 0;
	m_pClientList[iClientH]->m_iSpecialAbilityLastSec = 0;
	m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = 0;

	m_pClientList[iClientH]->m_iAddTransMana = 0;
	m_pClientList[iClientH]->m_iAddChargeCritical = 0;

	// re introduced by SNOOPY
	m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM = 0;
	m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L = 0;
	m_pClientList[iClientH]->m_iMagicHitRatio_ItemEffect = 0;

	m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
	// 착용하지 않아도 효과가 있는 아이템.
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) {
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0) {
					// 수명이 있어야 효과가 있다.
					m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
				}
				break;
			}
		}

	// Hidden bonus to hit per weapon reintroduced by SNOOPY
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) &&
			(m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == TRUE))
		{
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
			case DEF_ITEMEFFECTTYPE_ATTACK:
			case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:
			case DEF_ITEMEFFECTTYPE_DEFENSE:
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				break;
				// Use same (L) value for both SM & L
			case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
			case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				break;
			}
		}

	// 착용을 해야 효과가 있는 아이템 
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) &&
			(m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == TRUE)) {

			cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {

			case DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE:
				// 마법 데미지 절감 아이템. 인덱스를 저장한다.
				m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = sItemIndex;
				break;

			case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
			case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
			case DEF_ITEMEFFECTTYPE_ATTACK:
				m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
				m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
				m_pClientList[iClientH]->m_cAttackBonus_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
				m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
				m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
				// Xelima Upgraded weapons
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_cAttackBonus_SM += iTemp;
				m_pClientList[iClientH]->m_cAttackBonus_L += iTemp;
				// Notice: +1 on Magic damage as well
				m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;

				// Add skill to HitRatio
				m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];
				m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;

				// v1.41 Custom-Made <==> Manuf items
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != NULL)
				{
					m_pClientList[iClientH]->m_iCustomItemValue_Attack += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 2);
					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 100)
						m_pClientList[iClientH]->m_iCustomItemValue_Attack = 100;
					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < -100)
						m_pClientList[iClientH]->m_iCustomItemValue_Attack = -100;

					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 0)
					{	// Completion >100%
						dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
						dV1 = (dV2 / 100.0f) * (5.0f);
						m_pClientList[iClientH]->m_iMinAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM +
							m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;
						m_pClientList[iClientH]->m_iMinAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L +
							m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;
						if (m_pClientList[iClientH]->m_iMinAP_SM < 1) m_pClientList[iClientH]->m_iMinAP_SM = 1;
						if (m_pClientList[iClientH]->m_iMinAP_L < 1)  m_pClientList[iClientH]->m_iMinAP_L = 1;
						if (m_pClientList[iClientH]->m_iMinAP_SM > (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
							m_pClientList[iClientH]->m_iMinAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);
						if (m_pClientList[iClientH]->m_iMinAP_L > (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
							m_pClientList[iClientH]->m_iMinAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
					}
					else if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < 0)
					{	// Completion <100%
						dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
						dV1 = (dV2 / 100.0f) * (5.0f);
						m_pClientList[iClientH]->m_iMaxAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM
							+ m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;
						m_pClientList[iClientH]->m_iMaxAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L
							+ m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;
						if (m_pClientList[iClientH]->m_iMaxAP_SM < 1) m_pClientList[iClientH]->m_iMaxAP_SM = 1;
						if (m_pClientList[iClientH]->m_iMaxAP_L < 1)  m_pClientList[iClientH]->m_iMaxAP_L = 1;
						if (m_pClientList[iClientH]->m_iMaxAP_SM < (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
							m_pClientList[iClientH]->m_iMaxAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);
						if (m_pClientList[iClientH]->m_iMaxAP_L < (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
							m_pClientList[iClientH]->m_iMaxAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
					}
				}
				// Enchanted weapons
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
					case 2:  // Poison			-> Increase effect of 1st item, or replace a different effect
						break;
					case 1:  // Crit +			-> Increase effect of 1st item, or replace a different effect
					case 10: // CP (as wand)	-> Increase effect of 1st item, or replace a different effect
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
						}
						else
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						}
						break;
					case 7:  // Sharp
						m_pClientList[iClientH]->m_cAttackDiceRange_SM++;
						m_pClientList[iClientH]->m_cAttackDiceRange_L++;
						break;
					case 9:  // Ancient
						m_pClientList[iClientH]->m_cAttackDiceRange_SM += 2;
						m_pClientList[iClientH]->m_cAttackDiceRange_L += 2;
						break;
					case 11: // ManaConv 
						m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
						if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
						break;
					case 12: // Crit Increase 
						m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
						if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
						break;
					case 4:  // Magic-using weapons

						break;
					default: // All others
						m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
						m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						break;
					}
				}

				// Enchanted weapon, compute stats
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					switch (dwSWEType) {
					case 0:  break;
					case 1:  m_pClientList[iClientH]->m_iAddPR += (int)dwSWEValue * 7; break;
					case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // armes HitProba
					case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					case 4:  m_pClientList[iClientH]->m_iAddHP += (int)dwSWEValue * 7; break;
					case 5:  m_pClientList[iClientH]->m_iAddSP += (int)dwSWEValue * 7; break;
					case 6:  m_pClientList[iClientH]->m_iAddMP += (int)dwSWEValue * 7; break;
					case 7:  m_pClientList[iClientH]->m_iAddMR += (int)dwSWEValue * 7; break;
					case 8:  m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)dwSWEValue * 3; break;

					case 9:
						m_pClientList[iClientH]->m_iAddAbsMD += (int)dwSWEValue * 3;
						if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;

					case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case 11: m_pClientList[iClientH]->m_iAddExp += (int)dwSWEValue * 10; break;
					case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}
				}

				// Those Attack items where treated as a bundle above, see here special cases
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
				case DEF_ITEMEFFECTTYPE_ATTACK: // More combo for Flamberge
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue == 12)
					{
						m_pClientList[iClientH]->m_iAddCD += 5;
					}
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
					m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					// Xelima Upgraded weapons
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iSideEffect_MaxHPdown -= iTemp;
					if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown < 2) m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = 2;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
					// SNOOPY: For wands, ignore m_sItemEffectValue4/5/6 for damage{
					m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
					// Compute Mana save, max=80%
					m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
					if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
					//Snoopy: Reintroduced defence for staves
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					//Snoopy: Added Magic ToHit Bonus for some wands.
					m_pClientList[iClientH]->m_iMagicHitRatio_ItemEffect += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE: // PA for some weapons
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
					// TYpe of special weapon, Duration of the effect
					m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
					m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;
					// SNOOPY: put this here for Xelima / IceElemental Bows
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					if ((bNotify == TRUE) && (iEquipItemID == (int)sItemIndex))
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, NULL);
					break;
				}
				break;

			case DEF_ITEMEFFECTTYPE_ADDEFFECT:
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
				case 1:	// MR
					m_pClientList[iClientH]->m_iAddResistMagic += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddResistMagic += (iTemp * 10);
					break;

				case 2:	// MS
					m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iManaSaveRatio += iTemp;
					// MS max = 80%
					if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
					break;

				case 3:	// Adds physical damage 
					m_pClientList[iClientH]->m_iAddPhysicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddPhysicalDamage += iTemp;
					break;

				case 4:	// Rings Defence Ratio 
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;

					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);
					break;

				case 5:	// Lucky Gold Ring
					// SNOOPY: changed to use m_sItemEffectValue2 as Lucky % value
					//         and cumulative betxeen items
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 > 0)
						m_pClientList[iClientH]->m_bIsLuckyEffect += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					else m_pClientList[iClientH]->m_bIsLuckyEffect += 10;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_bIsLuckyEffect += (iTemp * 10);
					break;

				case 6:	// Add Magic Damage. 
					m_pClientList[iClientH]->m_iAddMagicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;
					break;

				case 7: // Lightning protection
					m_pClientList[iClientH]->m_iAddAbsAir += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsAir >= 0)		m_pClientList[iClientH]->m_iAddAbsAir += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsAir -= (iTemp * 5);
					break;
				case 8: // Earth Protection
					m_pClientList[iClientH]->m_iAddAbsEarth += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsEarth >= 0)	m_pClientList[iClientH]->m_iAddAbsEarth += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsEarth -= (iTemp * 5);
					break;
				case 9: // Fire Protection
					m_pClientList[iClientH]->m_iAddAbsFire += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsFire >= 0)		m_pClientList[iClientH]->m_iAddAbsFire += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsFire -= (iTemp * 5);
					break;
				case 10: // Ice Protection
					m_pClientList[iClientH]->m_iAddAbsWater += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsWater >= 0)	m_pClientList[iClientH]->m_iAddAbsWater += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsWater -= (iTemp * 5);
					break;

				case 11: // Poison protection
					m_pClientList[iClientH]->m_iAddPR += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddPR += (iTemp * 10);
					break;

				case 12: // Adds To Hit Bonus
					m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iHitRatio += (iTemp * 5);
					break;

				case 13: // Magin Ruby  Characters Hp recovery rate(% applied) added by the purity formula.
					m_pClientList[iClientH]->m_iAddHP += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 5);
					break;

				case 14: // Magin Diamond Attack probability(physical&magic) added by the purity formula.
					m_pClientList[iClientH]->m_iAddAR += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 5);
					break;

				case 15: // Magin Emerald Magical damage decreased(% applied) by the purity formula.    
					m_pClientList[iClientH]->m_iAddMagicalDamage += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 10);
					break;

					/*
					Functions rates confirm.
					Magic Diamond:	Completion rate / 5		= Functions rate. ? Maximum 20. (not%)
					Magic Ruby:		Completion rate / 5		= Functions rate.(%) ? Maximum 20%.
					Magic Emerald:	Completion rate / 10	= Functions rate.(%) ? Maximum 10%.
					Magic Sapphire: Completion rate / 10	= Functions rate.(%) ? Maximum 10%.
					*/

					// ******* Angel Code - Begin ******* //   
				case 16: // Angel STR//AngelicPandent(STR)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicStr = iTemp + 1;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 1, iTemp);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);
					break;
				case 17: // Angel DEX //AngelicPandent(DEX)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicDex = iTemp + 1;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 2, iTemp);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);
					break;
				case 18: // Angel INT//AngelicPandent(INT)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicInt = iTemp + 1;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 3, iTemp);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);
					break;
				case 19: // Angel MAG//AngelicPandent(MAG)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicMag = iTemp + 1;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 4, iTemp);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);
					break;
				}
				if (cEquipPos == DEF_EQUIPPOS_BACK)
				{	// Snoopy Special effect capes/ mantles still have small DefRatio & PA
					m_pClientList[iClientH]->m_iDefenseRatio += 3;	// Standard DR
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] ++;
					// Merien Upgraded diverse (capes?)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iTemp != 0)
					{
						m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);
						m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] += iTemp;
					}
				}
				break;

			case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:
				if ((m_pClientList[iClientH]->m_cArrowIndex != -1)
					&& (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cArrowIndex] == NULL))
				{
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);	// ArrowIndex
				}
				else if (m_pClientList[iClientH]->m_cArrowIndex == -1)
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

				if (m_pClientList[iClientH]->m_cArrowIndex == -1)
				{
					m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
					m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
					m_pClientList[iClientH]->m_cAttackBonus_SM = 0;
					m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
					m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
					m_pClientList[iClientH]->m_cAttackBonus_L = 0;
				}
				else
				{
					m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					m_pClientList[iClientH]->m_cAttackBonus_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
					m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
					m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
					m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
				}
				// Xelima Upgraded bows
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_cAttackBonus_SM += iTemp;
				m_pClientList[iClientH]->m_cAttackBonus_L += iTemp;

				// Standard HitRatio uses Skill 
				m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];
				// iHitRatio3 discards skill
				// SNOOPY: Added support for enchanted bows. (1st stat)
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
					case 2:  // Poison			-> Increase effect of 1st item, or replace a different effect					
						break;
					case 1:  // Crit +			-> Increase effect of 1st item, or replace a different effect
					case 10: // CP (as wand)	-> Increase effect of 1st item, or replace a different effect
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
						}
						else
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						}
						break;
					case 7:  // Sharp
						m_pClientList[iClientH]->m_cAttackDiceRange_SM++;
						m_pClientList[iClientH]->m_cAttackDiceRange_L++;
						break;
					case 9:  // Ancient
						m_pClientList[iClientH]->m_cAttackDiceRange_SM += 2;
						m_pClientList[iClientH]->m_cAttackDiceRange_L += 2;
						break;
					case 11: // ManaConv 
						m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
						if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
						break;
					case 12: // Crit Increase 
						m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
						if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
						break;
					case 4:  // Magic-using weapons, don't overide a main weapon

						break;
					default: // All others
						m_pClientList[iClientH]->m_iSpecialWeaponEffectType = (int)dwSWEType;
						m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = (int)dwSWEValue;
						break;
					}
				}

				// SNOOPY: Added support for enchanted bows. (2nd stat)
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					switch (dwSWEType) {
					case 0:  break;
					case 1:  m_pClientList[iClientH]->m_iAddPR += (int)dwSWEValue * 7; break;
					case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // adds Hit Proba
					case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					case 4:  m_pClientList[iClientH]->m_iAddHP += (int)dwSWEValue * 7; break;
					case 5:  m_pClientList[iClientH]->m_iAddSP += (int)dwSWEValue * 7; break;
					case 6:  m_pClientList[iClientH]->m_iAddMP += (int)dwSWEValue * 7; break;
					case 7:  m_pClientList[iClientH]->m_iAddMR += (int)dwSWEValue * 7; break;
					case 8:  m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)dwSWEValue * 3; break;
					case 9:  m_pClientList[iClientH]->m_iAddAbsMD += (int)dwSWEValue * 3;
						if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;
					case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case 11: m_pClientList[iClientH]->m_iAddExp += (int)dwSWEValue * 10; break;
					case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}
				}
				break;

			case DEF_ITEMEFFECTTYPE_DEFENSE: // Notice that MainGauche is here as well .....			
			case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
				// SNOOPY: 2 values for Defence ratio fot PvP mode HIGH
				m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;


				// Merien Upgraded armors, add DefRatio for all defence items
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);

				// Manufs armors 
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != NULL)
				{
					m_pClientList[iClientH]->m_iCustomItemValue_Defense += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					dV2 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					dV3 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					dV1 = (double)(dV2 / 100.0f) * dV3;
					dV1 = dV1 / 2.0f;
					m_pClientList[iClientH]->m_iDefenseRatio += (int)dV1;
					if (m_pClientList[iClientH]->m_iDefenseRatio <= 0) m_pClientList[iClientH]->m_iDefenseRatio = 1;
				}

				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
					case 1:  // Crit +			-> May increase effect of 1st item or give effect if no other effect present
					case 2:  // Poison			-> May Increase effect of 1st item or give effect if no other effect present
					case 10: // CP (as wand)	-> May Increase effect of 1st item or give effect if no other effect present				
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
						}
						else
							if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == 0)
							{
								m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
								m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
							}
						break;
					case 3: // Rite -> Give effect if no other effect present	
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == 0)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						}
						break;
					case 7: // Sharp MG have effect in iCalculatteAttackEffect function
					case 9: // Ancient MG have effect in iCalculatteAttackEffect function
						break;
					case 11: // ManaConv  Total max 20
						m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
						if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
						break;
					case 12: // Crit Increase Total max 20
						m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
						if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
						break;
					case 4: // ???
						break;
					default:
						break;
					}
				}
				// stated armors/shields
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != NULL)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					// HP or Rep+ will have effect on Armor or MainGauche !
					switch (dwSWEType) {
					case 0:  break;
					case 1:  m_pClientList[iClientH]->m_iAddPR += (int)dwSWEValue * 7; break;
					case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // Armes Hit Proba
					case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					case 4:  // HPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldHPrec += (int)dwSWEValue * 7;
						else m_pClientList[iClientH]->m_iAddHP += (int)dwSWEValue * 7;
						break;
					case 5:  // SPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldSPrec += (int)dwSWEValue * 7;
						else m_pClientList[iClientH]->m_iAddSP += (int)dwSWEValue * 7;
						break;
					case 6:   // MPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldMPrec += (int)dwSWEValue * 7;
						else m_pClientList[iClientH]->m_iAddMP += (int)dwSWEValue * 7;
						break;
					case 7:
						m_pClientList[iClientH]->m_iAddMR += (int)dwSWEValue * 7;
						break;
					case 8: // PA
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							m_pClientList[iClientH]->m_iDamageAbsorption_Shield += (int)dwSWEValue * 3;
						else m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)dwSWEValue * 3;
						break;
					case 9:
						m_pClientList[iClientH]->m_iAddAbsMD += (int)dwSWEValue * 3;
						if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;
					case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case 11: m_pClientList[iClientH]->m_iAddExp += (int)dwSWEValue * 10; break;
					case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}

				}
				switch (cEquipPos) { // Sub-cases of DEF_ITEMEFFECTTYPE_DEFENSE
				case DEF_EQUIPPOS_LHAND:
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 != 0)
					{
						m_pClientList[iClientH]->m_iDamageAbsorption_Shield = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					}
					else // Snoopy individual PA setting for shields...
					{
						m_pClientList[iClientH]->m_iDamageAbsorption_Shield = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) - (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) / 3;
					}
					// Merien Upgraded shield +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Shield += iTemp;

					break;
				case DEF_EQUIPPOS_TWOHAND:

				case DEF_EQUIPPOS_RHAND:

					break;
				case DEF_EQUIPPOS_HEAD:	// PA on Head
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded helmet +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] += iTemp;
					break;
				case DEF_EQUIPPOS_BODY:
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded armor +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += iTemp;
					switch (m_pClientList[iClientH]->m_cSex) {
					case 1: // Male  1:Leather, 5:Tunic
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) {
						case 1:  // Leather
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;

							break;
						case 5:  // Tunic
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;
							break;
						case 2:  // Chain
						case 3:  // Scale

							break;
						case 6: //  Robe(M)
						case 10: // eHeroRobe(M)
						case 11: // aHeroRobe(M) // Some all places PA with robes
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;

							break;
						case 4: // Plate
						default:	// plate +				

							break;
						}
						break;
					case 2: // Women 1,2:Bodice, 3:Leather
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) {
						case 1:  // Bodice
						case 2:  // Bodice
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;
							break;
						case 3:  // Leather
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;

							break;
						case 4:  // Chain
						case 5:  // Scale
							break;
						case 7: //  Robe(M)
						case 11: // eHeroRobe(M)
						case 12: // aHeroRobe(M) // Some all places PA with robes
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] / 2;
							break;
						case 6: // Plate
						default:

							break;
						}
						break;
					}
					break;
				case DEF_EQUIPPOS_ARMS:
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded hauberks +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] += iTemp;
					switch (m_pClientList[iClientH]->m_cSex) {
					case 1: // Male  1:Chemise 2:Hauberk
						if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue <= 1)
						{
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] / 2;
						}
						break;
					case 2: // Women 1,2:Chemise Shirt, 3:Hauberk
						if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue <= 2)
						{
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_RELEASEALL] = m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] / 2;
						}
						break;
					}
					break;
				case DEF_EQUIPPOS_PANTS:
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded leggings +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] += iTemp;
					break;
				case DEF_EQUIPPOS_LEGGINGS:
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded boots +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] += iTemp;
					// Motemorphosed char is still affected by original boots
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					break;
				case DEF_EQUIPPOS_BACK: // Cape
					// Motemorphosed char is still affected by original cape
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					break;

				default:
					// SNOOPY: Kept this here so we can use Damage absorbtion for something else (ie: Cape)
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded diverse (capes?)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += iTemp;
					break;
				}

				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
				case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
					m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
					m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;
					if ((bNotify == TRUE) && (iEquipItemID == (int)sItemIndex))
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, NULL);
					break;
				}
				break;
			}
		}

	//v1.432
	if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) && (bNotify == TRUE))
	{
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 4, NULL, NULL, NULL);
		if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == TRUE)
		{
			m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = FALSE;
			m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp & 0xFF0F;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
		}
	}
	if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType != 0)
		&& (iPrevSAType != m_pClientList[iClientH]->m_iSpecialAbilityType) && (bNotify == TRUE))
	{
		if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == TRUE)
		{
			SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, NULL, NULL, NULL); // 20min
			m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = FALSE;
			m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp & 0xFF0F;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
		}
	}

	// Snoopy! Limitation of HPrec, MPrec SPrec.100% max not counting shield			
	if (m_pClientList[iClientH]->m_iAddHP > 100) m_pClientList[iClientH]->m_iAddHP = 100;
	if (m_pClientList[iClientH]->m_iAddSP > 100) m_pClientList[iClientH]->m_iAddSP = 100;
	if (m_pClientList[iClientH]->m_iAddMP > 100) m_pClientList[iClientH]->m_iAddMP = 100;
	m_pClientList[iClientH]->m_iAddHP += iShieldHPrec;
	m_pClientList[iClientH]->m_iAddSP += iShieldSPrec;
	m_pClientList[iClientH]->m_iAddMP += iShieldMPrec;
	m_pClientList[iClientH]->m_cHeroArmourBonus = _cCheckHeroItemEquipped(iClientH);

	// Snoopy: Bonus for Angels	
	m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_iAngelicDex * 2;

	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH, FALSE);
	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);

	// Maximum value for  PA, here instead of iCalculateAttackEffect
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] > 60)	// Torse,  max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] > 60)	// T�te    max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] > 60)	// Membres max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] > 60)	// Membres max 60 
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] > 60)	// Pieds   max 60 
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] > 60)	// Dos     max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] = 60;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Shield > 60)						// Shield  max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 60;

	// Ne dire aux clients que si ca a chang�.
	if (iTotalAngelicstatas != (m_pClientList[iClientH]->m_iAngelicStr + (16 * m_pClientList[iClientH]->m_iAngelicInt) + (256 * m_pClientList[iClientH]->m_iAngelicDex) + (16 * 256 * m_pClientList[iClientH]->m_iAngelicMag)))
	{
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);
	}
}

BOOL CGame::_bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID)
{
	BOOL bRet;

	// ��짠횆징쨍짝 횁철횁짚횉횕째챠 쨩챌쩔챘 횊횆 쨩챌쨋처횁철쨈횂 쩐횈��횑횇횤 횊쩔째첬 횄쨀쨍짰 쨘횓쨘횖 
	if (m_pClientList[iClientH] == NULL) return FALSE;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;

	switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
	case DEF_ITEMEFFECTTYPE_OCCUPYFLAG:
		bRet = __bSetOccupyFlag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
			NULL, iClientH, NULL);
		if (bRet == TRUE) {
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTFLAGSPOT, NULL, NULL, NULL, NULL);
		}
		return bRet;

		// crusade
	case DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT:
		// 째횉쩌쨀 횇째횈짰��횑쨈횢. 쨔횑쨉챕쨌짙쨉책쩔징쩌짯쨍쨍 쨩챌쩔챘 째징쨈횋횉횚. m_sItemEffectValue1: 째횉횄횪쨔째 횁쩐쨌첫, m_sItemEffectValue2: 째횉횄횪 쩍횄째짙 
		bRet = __bSetConstructionKit(m_pClientList[iClientH]->m_cMapIndex, dX, dY,                                 // 
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,        //
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,        //
			iClientH); // 쩌쨀횆징��횣 
		if (bRet == TRUE) {
			// 째횉쩌쨀 쩍횄��횤	
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			// 째횉쩌쨀 쩍횉횈횖		
			// Centuu : No se puede construir.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, NULL, NULL, NULL, NULL);
		}
		return bRet;

	case DEF_ITEMEFFECTTYPE_DYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL) {
				if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 11) ||
					(m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 12))
				{
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
					return TRUE;
				}
				else 
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
					return FALSE;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_ARMORDYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL) {
				if (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 6) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
					return TRUE;
				}
				else {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
					return FALSE;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_WEAPONDYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL) {
				if (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 1) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
					return TRUE;
				}
				else {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
					return FALSE;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_FARMING:
		bRet = bPlantSeedBag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,
			iClientH);

		if (bRet == TRUE) {
			// 째횉쩌쨀 쩍횄��횤	
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			// 째횉쩌쨀 쩍횉횈횖	
			// Centuu : No se puede plantar.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMOREAGRICULTURE, NULL, NULL, NULL, NULL);
		}

		return bRet;

	default:
		break;
	}

	return TRUE;
}

// v1.4311-3 횄횩째징  횉횚쩌철 ��횚��책짹횉��쨩 횁횜쨈횂 횉횚쩌철   GetFightzoneTicketHandler 
void CGame::GetFightzoneTicketHandler(int iClientH)
{
	int   iRet, iEraseReq, iMonth, iDay, iHour;
	char* cp, cData[256], cItemName[21];
	class CItem* pItem;
	DWORD* dwp;
	short* sp;
	WORD* wp;

	if (m_pClientList[iClientH] == NULL) return;

	if (m_pClientList[iClientH]->m_iFightZoneTicketNumber <= 0) {
		// ��횚��책짹횉��쨩 쨈횢 쨩챌쩔챘횉횩��쩍��쨩 쩐횏쨍째쨈횢.
		// 쨩챌횇천��책 쨔첩횊짙째징 ��쩍쩌철쨍챕 쩔쨔쩐횪��쨘 횉횩쨈횂쨉짜 ��횚��책짹횉��쨩 쨈횢 쨔횧��쨘째챈쩔챙 ..
		m_pClientList[iClientH]->m_iFightzoneNumber *= -1;
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -1, NULL, NULL, NULL);
		return;
	}

	ZeroMemory(cItemName, sizeof(cItemName));

	if (m_pClientList[iClientH]->m_iFightzoneNumber == 1)
		strcpy(cItemName, "ArenaTicket");
	else  wsprintf(cItemName, "ArenaTicket(%d)", m_pClientList[iClientH]->m_iFightzoneNumber);

	pItem = new class CItem;
	if (_bInitItemAttr(pItem, cItemName) == FALSE) {
		delete pItem;
		return;
	}

	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
		// 쩔징쨌짱 쨔챈횁철쩔챘 횆횣쨉책
		if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

		// 쩐횈��횑횇횤��쨩 쨔횧��쨩 쩌철 ��횜쨈횢쨈횂 째횒��횑 횊짰횁짚 쨉횉쩐첬��쨍쨔횉쨌횓 ��횚��책짹횉째쨀쩌철쨍짝 쨩짤쨉쨉 쨉횊쨈횢. 
		m_pClientList[iClientH]->m_iFightZoneTicketNumber = m_pClientList[iClientH]->m_iFightZoneTicketNumber - 1;

		pItem->m_sTouchEffectType = DEF_ITET_DATE;

		iMonth = m_pClientList[iClientH]->m_iReserveTime / 10000;
		iDay = (m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000) / 100;
		iHour = m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000 - iDay * 100;

		pItem->m_sTouchEffectValue1 = iMonth;
		pItem->m_sTouchEffectValue2 = iDay;
		pItem->m_sTouchEffectValue3 = iHour;


		wsprintf(G_cTxt, "(*) Get FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)(%d)(%d)", m_pClientList[iClientH]->m_cCharName, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
		PutLogFileList(G_cTxt);
		PutLogList(G_cTxt);

		ZeroMemory(cData, sizeof(cData));

		// 쩐횈��횑횇횤 쩐챵쩐첬쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 ��체쩌횤횉횗쨈횢.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		// 1째쨀 횊쨔쨉챈횉횩쨈횢.
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		if (iEraseReq == 1) delete pItem;

		// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

		// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
		iCalcTotalWeight(iClientH);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}
	}
	else {
		// 째첩째짙��횑 쨘횓횁쨌횉횠 쩐횈��횑횇횤��쨩 쩐챵��쨩 쩌철 쩐첩쨈횢.
		delete pItem;

		// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
		iCalcTotalWeight(iClientH);

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}
	}
}

//Hero Code by Zabuza - fixed by Centu
void CGame::GetHeroMantleHandler(int iClientH, int iItemID, char* pString)
{
	int   i, iNum, iRet, iEraseReq;
	char* cp, cData[256], cItemName[21];
	class CItem* pItem;
	DWORD* dwp;
	short* sp;
	WORD* wp;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_iEnemyKillCount < 100) return;
	if (m_pClientList[iClientH]->m_iContribution < 10) return;
	if (m_pClientList[iClientH]->m_cSide == 0) return;

	if (_iGetItemSpaceLeft(iClientH) == 0) {
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
		return;
	}

	//Prevents a crash if item dosent exist
	if (m_pItemConfigList[iItemID] == NULL)  return;

	switch (iItemID) {
		// Hero Cape
	case 400: //Aresden HeroCape
	case 401: //Elvine HeroCape
		if (m_pClientList[iClientH]->m_iEnemyKillCount < 300) return;
		m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
		break;
	}

	// Da el item correspondiente dependiendo el sexo del personaje, centu.
	switch (m_pClientList[iClientH]->m_cSex) {
	case 1:
		switch (iItemID) {
			// Hero Helm
		case 403: //Aresden HeroHelm(M)
		case 405: //Elvine HeroHelm(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
			// Hero Cap
		case 407: //Aresden HeroCap(M)
		case 409: //Elvine HeroCap(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
			// Hero Armour
		case 411: //Aresden HeroArmour(M)
		case 413: //Elvine HeroArmour(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 300 || m_pClientList[iClientH]->m_iContribution < 30) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
			m_pClientList[iClientH]->m_iContribution -= 30;
			break;
			// Hero Robe
		case 415: //Aresden HeroRobe(M)
		case 417: //Elvine HeroRobe(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 200 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 200;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
			// Hero Hauberk
		case 419: //Aresden HeroHauberk(M)
		case 421: //Elvine HeroHauberk(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 10) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 10;
			break;
			// Hero Leggings
		case 423: //Aresden HeroLeggings(M)
		case 425: //Elvine HeroLeggings(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 15) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 15;
			break;
		default:
			break;
		}
		break;

	case 2:
		switch (iItemID) {
		case 404: //Aresden HeroHelm(W)
		case 406: //Elvine HeroHelm(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
		case 408: //Aresden HeroCap(W)
		case 410: //Elvine HeroCap(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
		case 412: //Aresden HeroArmour(W)
		case 414: //Elvine HeroArmour(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 300 || m_pClientList[iClientH]->m_iContribution < 30) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
			m_pClientList[iClientH]->m_iContribution -= 30;
			break;
		case 416: //Aresden HeroRobe(W)
		case 418: //Elvine HeroRobe(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 200 || m_pClientList[iClientH]->m_iContribution < 20) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 200;
			m_pClientList[iClientH]->m_iContribution -= 20;
			break;
		case 420: //Aresden HeroHauberk(W)
		case 422: //Elvine HeroHauberk(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 10) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 10;
			break;
		case 424: //Aresden HeroLeggings(W)
		case 426: //Elvine HeroLeggings(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 15) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 15;
			break;

		default:
			break;
		}
		break;
	}

	ZeroMemory(cItemName, sizeof(cItemName));
	memcpy(cItemName, m_pItemConfigList[iItemID]->m_cName, 20);
	// ReqPurchaseItemHandler
	iNum = 1;
	for (i = 1; i <= iNum; i++) {
		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == FALSE) {
			delete pItem;
		}
		else {
			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				wsprintf(G_cTxt, "(*) Get HeroItem : Char(%s) Player-EK(%d) Player-Contr(%d) Hero Obtained(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iEnemyKillCount, m_pClientList[iClientH]->m_iContribution, cItemName);
				PutLogFileList(G_cTxt);

				pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_ITEMOBTAINED;
				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

				*cp = 1;
				cp++;

				memcpy(cp, pItem->m_cName, 20);
				cp += 20;

				dwp = (DWORD*)cp;
				*dwp = pItem->m_dwCount;
				cp += 4;

				*cp = pItem->m_cItemType;
				cp++;

				*cp = pItem->m_cEquipPos;
				cp++;

				*cp = (char)0;
				cp++;

				sp = (short*)cp;
				*sp = pItem->m_sLevelLimit;
				cp += 2;

				*cp = pItem->m_cGenderLimit;
				cp++;

				wp = (WORD*)cp;
				*wp = pItem->m_wCurLifeSpan;
				cp += 2;

				wp = (WORD*)cp;
				*wp = pItem->m_wWeight;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSprite;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSpriteFrame;
				cp += 2;

				*cp = pItem->m_cItemColor;
				cp++;

				*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
				cp++;

				dwp = (DWORD*)cp;
				*dwp = pItem->m_dwAttribute;
				cp += 4;

				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, NULL, NULL, NULL);

				if (iEraseReq == 1) delete pItem;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

				iCalcTotalWeight(iClientH);

				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}


			}
			else {
				delete pItem;

				iCalcTotalWeight(iClientH);

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
			}
		}
	}
}

void CGame::_SetItemPos(int iClientH, char* pData)
{
	char* cp, cItemIndex;
	short* sp, sX, sY;

	if (m_pClientList[iClientH] == NULL) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cItemIndex = *cp;
	cp++;

	sp = (short*)cp;
	sX = *sp;
	cp += 2;

	sp = (short*)cp;
	sY = *sp;
	cp += 2;

	// ��횩쨍첩쨉횊 횁횂횉짜째짧 쨘쨍횁짚 
	if (sY < -10) sY = -10;

	if ((cItemIndex < 0) || (cItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemIndex] != NULL) {
		m_pClientList[iClientH]->m_ItemPosList[cItemIndex].x = sX;
		m_pClientList[iClientH]->m_ItemPosList[cItemIndex].y = sY;
	}
}

void CGame::CheckUniqueItemEquipment(int iClientH)
{
	int i, iDamage;

	if (m_pClientList[iClientH] == NULL) return;
	// v1.3 ��짱쨈횕횇짤 쩐횈��횑횇횤��횉 ��책횂첩 쨩처횇횂쨍짝 횈횉쨈횥횉횗쨈횢. 횈짱횁짚��횓쨍쨍��횑 횂첩쩔챘 째징쨈횋횉횗 쩐횈��횑횇횤��쨩 ��책횂첩횉횗 째챈쩔챙 쨔첸째횥횁첩쨈횢.

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
			if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER) &&
				(m_pClientList[iClientH]->m_bIsItemEquipped[i] == TRUE)) {
				// Touch Effect Type��횑 DEF_ITET_OWNER��횑쨍챕 Touch Effect Value 1, 2, 3��횑 횁횜��횓 횆쨀쨍짱횇횒��횉 째챠��짱째짧��쨩 째짰쨈횂쨈횢. 

				if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
					// ID 째짧��횑 쨍횂쨈횢.

					// ��횣쩍횇��횉 째횒��횑 쩐횈쨈횕쨔횉쨌횓 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, NULL, NULL);
					ReleaseItemHandler(iClientH, i, TRUE);
					// v1.4 쨈챘쨔횑횁철쨍짝 쩐챵쨈횂쨈횢. 
					iDamage = iDice(1, 100); // Centuu
					m_pClientList[iClientH]->m_iHP -= iDamage;
					if (m_pClientList[iClientH]->m_iHP <= 0) {
						ClientKilledHandler(iClientH, NULL, NULL, iDamage);
					}
				}
			}
		}
}

/*********************************************************************************************************************
** BOOL CGame::GetAngelHandler(int iClientH, char * pData, DWORD dwMsgSize)											**
** description   :: Reversed and coded by Snoopy																	**
*********************************************************************************************************************/
void CGame::GetAngelHandler(int iClientH, char* pData, DWORD dwMsgSize)
{
	char* cp, cData[256], cTmpName[21];
	int   iAngel, iItemNbe;
	class CItem* pItem;
	int   iRet, iEraseReq;
	short* sp;
	WORD* wp;
	int* ip;
	DWORD* dwp;
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	// Centuu: Razones distintas por la cu�l no puedes obtener un angel.
	if (_iGetItemSpaceLeft(iClientH) == 0)
	{
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ANGEL_FAILED, 1, NULL, NULL, NULL); //"Impossible to get a Tutelary Angel."
		return;
	}
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 5)
	{
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ANGEL_FAILED, 2, NULL, NULL, NULL); //"You need additional Majesty Points."	
		return;
	}
	if (m_pClientList[iClientH]->m_iLevel < m_iPlayerMaxLevel)
	{
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ANGEL_FAILED, 3, NULL, NULL, NULL); //"Only Majesty characters can receive Tutelary Angel."
		return;
	}
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, cp);
	cp += 20;
	ip = (int*)cp;
	iAngel = (int)*ip; // 0x00 l a i
	cp += 2;
	switch (iAngel) {
	case 1: // STR
		iItemNbe = 908;
		break;
	case 2: // DEX
		iItemNbe = 909;
		break;
	case 3: // INT
		iItemNbe = 910;
		break;
	case 4: // MAG
		iItemNbe = 911;
		break;
	default:
		PutLogList("Gail asked to create a wrong item!");
		break;
	}
	pItem = new class CItem;
	if (pItem == NULL) return;
	if ((_bInitItemAttr(pItem, iItemNbe) == TRUE))
	{
		pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
		pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
		pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
		pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
		{	
			ZeroMemory(cData, sizeof(cData));
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMOBTAINED;
			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			*cp = 1;
			cp++;
			memcpy(cp, pItem->m_cName, 20);
			cp += 20;
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;
			*cp = pItem->m_cItemType;
			cp++;
			*cp = pItem->m_cEquipPos;
			cp++;
			*cp = (char)0;
			cp++;
			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;
			*cp = pItem->m_cGenderLimit;
			cp++;
			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;
			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;
			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;
			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;
			*cp = pItem->m_cItemColor;
			cp++;
			*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
			cp++;
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;
			if (iEraseReq == 1) delete pItem;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= 5;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);
			// Centuu : Recibe el angel.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ANGEL_RECEIVED, NULL, NULL, NULL, NULL); //"You have received the Tutelary Angel."
		}
		else
		{
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4   
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
		}
	}
	else
	{
		delete pItem;
		pItem = NULL;
	}
}

/*********************************************************************************************************************
**  void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)											**
**  DESCRIPTION			:: set exchange item																		**
**  LAST_UPDATED		:: March 20, 2005; 6:31 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- make sure there that the item player places in trade exists							**
**							- stores item number in other character													**
**							- counts total item numbers sent														**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)
{
	int iExH, i;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
	if (m_pClientList[iClientH]->m_iExchangeCount > 8) return;	//only 4 items trade

	//no admin trade
	if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) {
		_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
		_ClearExchangeStatus(iClientH);
	}

	if ((m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) && (m_pClientList[iClientH]->m_iExchangeH != NULL)) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;
		if ((m_pClientList[iExH] == NULL) || (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0)) {
			// 교환할 상대방이 없거나 교환하고자 했던 그 캐릭터가 아니다. 	
		}
		else {
			// 교환하고자 하는 상대방에게 아이템을 알려준다. 
			// 해당 아이템이 존재하는지, 수량이 맞는지 체크한다. 
			if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount < iAmount) return;

			//No Duplicate items
			for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount;i++) {
				if (m_pClientList[iClientH]->m_cExchangeItemIndex[i] == (char)iItemIndex) {
					_ClearExchangeStatus(iExH);
					_ClearExchangeStatus(iClientH);
					return;
				}
			}

			// 교환하고자 하는 아이템 인덱스, 수량 저장 
			m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->m_iExchangeCount] = (char)iItemIndex;
			m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->m_iExchangeCount] = iAmount;

			memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->m_iExchangeCount], m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);



			m_pClientList[iClientH]->m_iExchangeCount++;
			SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);

			SendNotifyMsg(iClientH, iExH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ConfirmExchangeItem(int iClientH)																	**
**  DESCRIPTION			:: confirms exchange																		**
**  LAST_UPDATED		:: March 20, 2005; 5:51 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- trade cancels if player changes HGServer												**
**							- trade cancels if player is disconnected or in waiting process							**
**							- trade cancels if player is trading with himself										**
**							- trade cancels if player is trading with noone											**
**							- trade cancels if player is recieves an item which hasnt been traded					**
**							- trade cancels if other player receives an item which you didn't trade					**
**							- changed weight calculations to support multiple items									**
**							- trade cancels if there is not enough item space for trade to occur					**
**  MODIFICATION		::	- GM can't trade if admin-security = true												**
**							- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::ConfirmExchangeItem(int iClientH)
{
	int iExH, i;
	int iItemWeightA, iItemWeightB, iWeightLeftA, iWeightLeftB, iAmountLeft;
	class CItem* pItemA[8], * pItemB[8], * pItemAcopy[8], * pItemBcopy[8];

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
	if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;

	if ((m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) && (m_pClientList[iClientH]->m_iExchangeH != NULL)) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;

		// v1.42 설마 이런 경우가?
		if (iClientH == iExH) return;

		if (m_pClientList[iExH] != NULL) {
			if ((memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ||
				(m_pClientList[iExH]->m_bIsExchangeMode != TRUE) ||
				(memcmp(m_pClientList[iExH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName, 10) != 0)) {
				// 교환하고자 했던 그 캐릭터가 아니다. 교환 상태는 취소.
				_ClearExchangeStatus(iClientH);
				_ClearExchangeStatus(iExH);
				return;
			}
			else {
				m_pClientList[iClientH]->m_bIsExchangeConfirm = TRUE;
				if (m_pClientList[iExH]->m_bIsExchangeConfirm == TRUE) {
					// 상대방도 교환의사를 밝혔다. 교환이 성립할 수 있는지 계산한다. 아이템 갯수, 무게 제한을 판단.

					//Check all items
					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] == NULL) ||
							(memcmp(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iClientH]->m_cExchangeItemName[i], 20) != 0)) {
							_ClearExchangeStatus(iClientH);
							_ClearExchangeStatus(iExH);
							return;
						}
					}
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] == NULL) ||
							(memcmp(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iExH]->m_cExchangeItemName[i], 20) != 0)) {
							_ClearExchangeStatus(iClientH);
							_ClearExchangeStatus(iExH);
							return;
						}
					}

					iWeightLeftA = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);
					iWeightLeftB = _iCalcMaxLoad(iExH) - iCalcTotalWeight(iExH);

					//Calculate weight for items
					iItemWeightA = 0;
					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						iItemWeightA = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]],
							m_pClientList[iClientH]->m_iExchangeItemAmount[i]);
					}
					iItemWeightB = 0;
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						iItemWeightB = iGetItemWeight(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]],
							m_pClientList[iExH]->m_iExchangeItemAmount[i]);
					}

					//See if the other person can take the item weightload
					if ((iWeightLeftA < iItemWeightB) || (iWeightLeftB < iItemWeightA)) {
						// 교환하고자 하는 아이템을 받을 무게공간이 없다. 교환 불가능. 
						_ClearExchangeStatus(iClientH);
						_ClearExchangeStatus(iExH);
						return;
					}

					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						// 수량단위가 있는 아이템의 경우는 새로 생성을 시켜야 나눌수 있다.
						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {

							if (m_pClientList[iClientH]->m_iExchangeItemAmount[i] > m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount) {
								// 교환하고자 했던 수량보다 아이템이 적다. 그동안 줄어들었다.
								_ClearExchangeStatus(iClientH);
								_ClearExchangeStatus(iExH);
								return;
							}
							pItemA[i] = new class CItem;
							_bInitItemAttr(pItemA[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];

							// 로그를 남기기 위한 아이템 복사물 
							pItemAcopy[i] = new class CItem;
							_bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemAcopy[i], pItemA[i]);
							pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
						}
						else {
							pItemA[i] = (class CItem*)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]];
							pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];

							// 로그를 남기기 위한 아이템 복사물 
							pItemAcopy[i] = new class CItem;
							_bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemAcopy[i], pItemA[i]);
							pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
						}
					}

					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {

							if (m_pClientList[iExH]->m_iExchangeItemAmount[i] > m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount) {
								// 교환하고자 했던 수량보다 아이템이 적다. 그동안 줄어들었다.
								_ClearExchangeStatus(iClientH);
								_ClearExchangeStatus(iExH);
								return;
							}
							pItemB[i] = new class CItem;
							_bInitItemAttr(pItemB[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];

							// 로그를 남기기 위한 아이템 복사물 
							pItemBcopy[i] = new class CItem;
							_bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemBcopy[i], pItemB[i]);
							pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
						}
						else {
							pItemB[i] = (class CItem*)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]];
							pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];

							// 로그를 남기기 위한 아이템 복사물 
							pItemBcopy[i] = new class CItem;
							_bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemBcopy[i], pItemB[i]);
							pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
						}
					}

					// 먼저 아이템을 넣은 후 
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						bAddItem(iClientH, pItemB[i], NULL);
						_bItemLog(DEF_ITEMLOG_EXCHANGE, iExH, iClientH, pItemBcopy[i]);
						delete pItemBcopy[i];
						pItemBcopy[i] = NULL;
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
							//
							iAmountLeft = (int)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iExH]->m_iExchangeItemAmount[i];
							if (iAmountLeft < 0) iAmountLeft = 0;
							// v1.41 !!!
							SetItemCount(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], iAmountLeft);

						}
						else {
							// 만약 장착된 아이템이라면 해제한다.
							ReleaseItemHandler(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], TRUE);
							SendNotifyMsg(NULL, iExH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iExH]->m_cExchangeItemIndex[i], m_pClientList[iExH]->m_iExchangeItemAmount[i], NULL, m_pClientList[iClientH]->m_cCharName);
							m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] = NULL;
						}
					}

					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						bAddItem(iExH, pItemA[i], NULL);
						_bItemLog(DEF_ITEMLOG_EXCHANGE, iClientH, iExH, pItemAcopy[i]);
						delete pItemAcopy[i];
						pItemAcopy[i] = NULL;

						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
							//
							iAmountLeft = (int)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iClientH]->m_iExchangeItemAmount[i];
							if (iAmountLeft < 0) iAmountLeft = 0;
							// v1.41 !!!
							SetItemCount(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], iAmountLeft);

						}
						else {
							// 만약 장착된 아이템이라면 해제한다.
							ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], TRUE);
							SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iClientH]->m_cExchangeItemIndex[i], m_pClientList[iClientH]->m_iExchangeItemAmount[i], NULL, m_pClientList[iExH]->m_cCharName);
							m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] = NULL;
						}
					}

					m_pClientList[iClientH]->m_bIsExchangeMode = FALSE;
					m_pClientList[iClientH]->m_bIsExchangeConfirm = FALSE;
					ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
					m_pClientList[iClientH]->m_iExchangeH = NULL;
					m_pClientList[iClientH]->m_iExchangeCount = 0;

					m_pClientList[iExH]->m_bIsExchangeMode = FALSE;
					m_pClientList[iExH]->m_bIsExchangeConfirm = FALSE;
					ZeroMemory(m_pClientList[iExH]->m_cExchangeName, sizeof(m_pClientList[iExH]->m_cExchangeName));
					m_pClientList[iExH]->m_iExchangeH = NULL;
					m_pClientList[iExH]->m_iExchangeCount = 0;

					for (i = 0;i < 8;i++) {
						m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
						m_pClientList[iExH]->m_cExchangeItemIndex[i] = -1;
					}

					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, NULL, NULL, NULL, NULL);
					SendNotifyMsg(NULL, iExH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, NULL, NULL, NULL, NULL);

					// 무게 재설정
					iCalcTotalWeight(iClientH);
					iCalcTotalWeight(iExH);
					return;
				}
			}
		}
		else {
			// 교환할 상대방이 없다. 교환은 취소 된다.
			_ClearExchangeStatus(iClientH);
			return;
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount,									**
**									short dX, short dY, WORD wObjectID, char *pItemName)							**
**  DESCRIPTION			:: exchange item handler																	**
**  LAST_UPDATED		:: March 20, 2005; 6:31 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- stores item number																	**
**							- counts total item numbers																**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char* pItemName)
{
	short sOwnerH;
	char  cOwnerType;
	int i;

	if (m_pClientList[iClientH] == NULL) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount < iAmount) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
	if (m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) return;
	if (wObjectID >= DEF_MAXCLIENTS) return;

	// 아이템을 누구와 교환하겠다는 메시지가 도착했다. 상대방에게 알리고 양쪽 모두 교환창을 열도록 한다. 
	//이제 dX, dY에 있는 오브젝트에게 소비성 아이템을 건네준다. 
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

	if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_PLAYER)) {

		if ((m_bAdminSecurity == TRUE) && (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0)) return;

		// v1.4 주고자 한 객체와 맞는지 판단한다.
		if (wObjectID != NULL) {
			if (wObjectID < 10000) {
				// 플레이어 
				if (m_pClientList[wObjectID] != NULL) {
					if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
				}
			}
			else sOwnerH = NULL;
		}

		if ((sOwnerH == NULL) || (m_pClientList[sOwnerH] == NULL)) {
			//쩐횈��횑횇횤 짹쨀횊짱��쨩 쩔채횄쨩횉횗 횉횄쨌쨔��횑쩐챤쩔징째횚 횉횠쨈챌 ��짠횆징쩔징 횉횄쨌쨔��횑쩐챤째징 쩐첩��쩍��쨩 쩐횏쨍째쨈횢. 
			_ClearExchangeStatus(iClientH);
		}
		else {
			if ((m_pClientList[sOwnerH]->m_bIsExchangeMode == TRUE) || (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) ||
				(m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->m_bIsFightZone == TRUE)) {
				// 쨩처쨈챘쨔챈��횑 ��횑쨔횑 짹쨀횊짱 횁횩��횑째횇쨀짧 ��체횇천쨍챨쨉책, 횊짚��쨘 쨩챌횇천��책��횑쨈횢. 짹쨀횊짱쨍챨쨉책쨌횓 쨉챕쩐챤째짜 쩌철 쩐첩쨈횢. 
				_ClearExchangeStatus(iClientH);
			}
			else {
				// 짹쨀횊짱쨍챨쨉책째징 쩍횄��횤쨉횉쩐첬쨈횢. ��횓쨉짝쩍쨘, ��횑쨍짠 ��첬��책  
				m_pClientList[iClientH]->m_bIsExchangeMode = TRUE;
				m_pClientList[iClientH]->m_iExchangeH = sOwnerH;
				ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
				strcpy(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[sOwnerH]->m_cCharName);

				//Clear items in the list
				m_pClientList[iClientH]->m_iExchangeCount = 0;
				m_pClientList[sOwnerH]->m_iExchangeCount = 0;
				for (i = 0; i < 8; i++) {
					//Clear the trader
					ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName[i], sizeof(m_pClientList[iClientH]->m_cExchangeItemName[i]));
					m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
					m_pClientList[iClientH]->m_iExchangeItemAmount[i] = 0;
					//Clear the guy we're trading with
					ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeItemName[i], sizeof(m_pClientList[sOwnerH]->m_cExchangeItemName[i]));
					m_pClientList[sOwnerH]->m_cExchangeItemIndex[i] = -1;
					m_pClientList[sOwnerH]->m_iExchangeItemAmount[i] = 0;
				}

				// 짹쨀횊짱횉횕째챠��횣 횉횕쨈횂 쩐횈��횑횇횤 ��횓쨉짝쩍쨘, 쩌철쨌짰 ��첬��책 
				m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->m_iExchangeCount] = (char)sItemIndex;
				m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->m_iExchangeCount] = iAmount;

				memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->m_iExchangeCount], m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, 20);

				m_pClientList[sOwnerH]->m_bIsExchangeMode = TRUE;
				m_pClientList[sOwnerH]->m_iExchangeH = iClientH;
				ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeName, sizeof(m_pClientList[sOwnerH]->m_cExchangeName));
				strcpy(m_pClientList[sOwnerH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName);

				m_pClientList[iClientH]->m_iExchangeCount++;
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

				SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
			}
		}
	}
	else {
		// NPC쩔횒쨈횂 쨔째째횉��쨩 짹쨀횊짱횉횘 쩌철 쩐첩쨈횢.
		_ClearExchangeStatus(iClientH);
	}
}

/*********************************************************************************************************************
**  void CGame::_ClearExchangeStatus(int iToH)																		**
**  DESCRIPTION			:: clear exchange status																	**
**  LAST_UPDATED		:: March 20, 2005; 7:15 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- clears all trade variables															**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::_ClearExchangeStatus(int iToH)
{
	if ((iToH <= 0) || (iToH >= DEF_MAXCLIENTS)) return;
	if (m_pClientList[iToH] == NULL) return;

	if (m_pClientList[iToH]->m_cExchangeName != FALSE)
		SendNotifyMsg(NULL, iToH, DEF_NOTIFY_CANCELEXCHANGEITEM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	m_pClientList[iToH]->m_dwInitCCTime = FALSE;
	m_pClientList[iToH]->m_iAlterItemDropIndex = 0;
	m_pClientList[iToH]->m_iExchangeH = NULL;

	m_pClientList[iToH]->m_bIsExchangeMode = FALSE;
	m_pClientList[iToH]->m_bIsExchangeConfirm = FALSE;

	ZeroMemory(m_pClientList[iToH]->m_cExchangeName, sizeof(m_pClientList[iToH]->m_cExchangeName));
}

void CGame::CancelExchangeItem(int iClientH)
{
	_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
	_ClearExchangeStatus(iClientH);
}

void CGame::RequestItemUpgradeHandler(int iClientH, int iItemIndex)
{
	int i, iItemX, iItemY, iSoM, iSoX, iSomH, iSoxH, iValue; // v2.172
	DWORD dwTemp, dwSWEType;
	double dV1, dV2, dV3;
	int sItemUpgrade = 2;

	if (m_pClientList[iClientH] == NULL) return;
	if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return;
	
	if (HeroItemChecker(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum, 0, 0, 0) != 0) 
	{
		if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
			(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
			(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
			return;
		}
		if (iUpgradeHeroCapeRequirements(iClientH, iItemIndex)) {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_UPGRADEHEROCAPE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], FALSE);
		}
		else {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], FALSE);
		}
		return;
	}

	iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
	switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cCategory)
	{
		case 46: // Pendants are category 46
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType != 1) 
			{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);			
				return; // Pendants are type 1
			}
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cEquipPos < 11) 
			{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);			
				return; // Pendants are left finger or more
			}
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType != 14) 
			{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);			
				return; // Pendants are EffectType 14
			}
			switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectValue1) 
			{
			default: // Other items are not upgradable
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);			
				return; // Pendants are EffectType 14

			case 16: // AngelicPandent(STR)
			case 17: // AngelicPandent(DEX)
			case 18: // AngelicPandent(INT)
			case 19: // AngelicPandent(MAG)
				if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) 
				{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
					return;
				}
				if (iValue == 10)
				{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, NULL, NULL, NULL);
					return;
				}
				switch (iValue) {
					case 0: sItemUpgrade = 10; break;
					case 1: sItemUpgrade = 11; break;
					case 2: sItemUpgrade = 13; break;
					case 3: sItemUpgrade = 16; break;
					case 4: sItemUpgrade = 20; break;
					case 5: sItemUpgrade = 25; break;
					case 6: sItemUpgrade = 31; break;
					case 7: sItemUpgrade = 38; break;
					case 8: sItemUpgrade = 46; break;
					case 9: sItemUpgrade = 55; break;
				}
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1)
					|| (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2)
					|| (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
					return;
				}
				if (( m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade ) < 0) 
				{	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
					return; 
				}
					m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade; 
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);
					iValue++;
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int) -1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				
				break;
			}
			break;

	case 1:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
		{
		case 709: // DarkKnightFlameberge
		case 737: // DarkKnightGiantSword
		case 745: // BlackKnightTemple
		case 989: // BlackKnightHammer
		case 990: // BlackKnightBH
		case 991: // BlackKnightBBH
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) 
			{
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}

			if (iValue == 15) 
			{
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, NULL, NULL, NULL);
				return;
			}

			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType != 0) 
			{
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
					return;
				}
			}

			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
			{
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}

			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

			if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 709) )
			{ // DarkKnightFlameberge
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 737) == FALSE) 
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 737))  // DarkKnightGiantSword
			{
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 745) == FALSE) 
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 745))  //BlackKnightTemple
			{
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 989))
			{ // BlackKnightHammer
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 990) == FALSE)
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 990))  // BlackKnightBH
			{
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 991) == FALSE)
				{
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 991))  //BlackKnightBBH
			{
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else 
			{
				iValue += 2;
				if (iValue > 15) iValue = 15;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			break;

		case 717: // DarkKnightRapier
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}
			if (iValue == 7) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, NULL, NULL, NULL);
				return;
			}
			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
				return;
			}
			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);
			iValue++;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			break;

		default:
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
				if (dwSWEType == 9) {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
					return;
				}
			}
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break;
					case 657: iSoM++; iSomH = i; break;
					}
				}
			if (iSoX > 0) {
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == FALSE) {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE);
					ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
					return;
				}
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) {
					iValue++;
					if (iValue > 10) {
						iValue = 10;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
					}
				}
				else {
					iValue++;
					if (iValue > 7) {
						iValue = 7;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
					}
				}
			}
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
			break;
		}
		return;

	case 3:
		if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL) {
			dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
			if (dwSWEType == 9) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
				return;
			}
		}
		iSoX = iSoM = 0;
		for (i = 0; i < DEF_MAXITEMS; i++)
			if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
				switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
				case 656: iSoX++; iSoxH = i; break;
				case 657: iSoM++; iSomH = i; break;
				}
			}
		if (iSoX > 0) {
			if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == FALSE) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE);
				ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
				return;
			}
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) {
				iValue++;
				if (iValue > 10) {
					iValue = 10;
				}
				else {
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
					ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
				}
			}
			else {
				iValue++;
				if (iValue > 7) {
					iValue = 7;
				}
				else {
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
					ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
				}
			}
		}
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
		return;

	case 5:
		if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL) {
			dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
			if (dwSWEType == 8) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
				return;
			}
		}
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
		case 623:
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
			return;
		default:
			break;
		}
		iSoX = iSoM = 0;
		for (i = 0; i < DEF_MAXITEMS; i++)
			if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
				switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
				case 656: iSoX++; iSoxH = i; break;
				case 657: iSoM++; iSomH = i; break;
				}
			}
		if (iSoM > 0) {
			if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, TRUE) == FALSE) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE);
				ItemDepleteHandler(iClientH, iSomH, FALSE, TRUE);
				return;
			}
			iValue++;
			if (iValue > 10) {
				iValue = 10;
			}
			else {
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) {
					dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					dV2 = 0.2f * dV1;
					dV3 = dV1 + dV2;
				}
				else {
					dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					dV2 = 0.15f * dV1;
					dV3 = dV1 + dV2;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
				if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0) m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan += dV2;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CURLIFESPAN, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, NULL, NULL);
				ItemDepleteHandler(iClientH, iSomH, FALSE, TRUE);
			}
		}
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, NULL, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
		return;

	case  6:
	case 15:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
		case 621: // MerienPlateMailM
		case 622: // MerienPlateMailW 
		case 700: // SangAhHauberk 
		case 701: // SangAhFullHel
		case 702: // SangAhLeggings  
		case 704: // SangAhPlateMail 
		case 706: // DarkKnightHauberk
		case 707: // DarkKnightFullHelm 
		case 708: // DarkKnightLeggings  
		case 710: // DarkKnightPlateMail 
		case 711: // DarkMageHauberk 
		case 712: // DarkMageChainMail
		case 713: // DarkMageLeggings  
		case 716: // DarkMageLedderArmor
		case 719: // DarkMageScaleMail
		case 724: // DarkKnightHauberkW
		case 725: // DarkKnightFullHelmW 
		case 726: // DarkKnightLeggingsW 
		case 728: // DarkKnightPlateMailW 
		case 729: // DarkMageHauberkW 
		case 730: // DarkMageChainMailW 
		case 731: // DarkMageLeggingsW 
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
			return;

		default:
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
				if (dwSWEType == 8) {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
					return;
				}
			}
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break;
					case 657: iSoM++; iSomH = i; break;
					}
				}
			if (iSoM > 0) {
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, TRUE) == FALSE) {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE);
					ItemDepleteHandler(iClientH, iSomH, FALSE, TRUE);
					return;
				}
				iValue++;
				if (iValue > 10) {
					iValue = 10;
				}
				else {
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
					if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) {
						dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
						dV2 = 0.2f * dV1;
						dV3 = dV1 + dV2;
					}
					else {
						dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
						dV2 = 0.15f * dV1;
						dV3 = dV1 + dV2;
					}
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
					if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0) m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan += dV2;
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CURLIFESPAN, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, NULL, NULL);
					ItemDepleteHandler(iClientH, iSomH, FALSE, TRUE);
				}
			}
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, NULL, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);

			break;
		}
		return;

	case 8:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
			// centu - MagicWand(MS30-LLF) ??? 
		case 714: // DarkMageMagicStaff 
		case 732: // DarkMageMagicStaffW 
		case 738: // DarkMageMagicWand
		case 746: // BlackMageTemple
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
				return;
			}
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}
			if (iValue == 15) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, NULL, NULL, NULL);
				return;
			}
			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
				return;
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);
			if (iValue == 0) {
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			}
			if ((iValue >= 4) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 714) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 732))) {
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 738) == FALSE) { // DarkMageMagicWand
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 738)) {
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 746) == FALSE) { // BlackMageTemple 
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 746)) {
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else {
				iValue += 2;
				if (iValue > 15) iValue = 15;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF; // 쨘챰횈짰 횇짭쨍짰쩐챤 
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28); // 쩐첨짹횞쨌쨔��횑쨉책쨉횊 쨘챰횈짰째짧 ��횚쨌횂
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			break;

		default:
			// ��횕쨔횦 쩐횈��횑횇횤
			// v2.16 2002-5-21 째챠짹짚횉철쩌철횁짚
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break; // 쩍쨘횇챈 쩔��쨘챗 횁짝쨍짰쨍쨋 
					case 657: iSoM++; iSomH = i; break; // 쩍쨘횇챈 쩔��쨘챗 쨍횧쨍짰쩔짙 
					}
				}
			// 쩍쨘횇챈 쩔��쨘챗 횁짝쨍짰쨍쨋째징 ��횜쨈횢.
			if (iSoX > 0) {
				// 쩐첨짹횞쨌쨔��횑쨉책 쩌쨘째첩 횊짰쨌체 째챔쨩챗.
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == FALSE) {
					// 쩐첨짹횞쨌쨔��횑쨉책 쩍횉횈횖 
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE); // v2.172 +1 -> +2 쨈횥째챔쩔징쩌짯 쩍횉횈횖횉횕쨍챕 쨩챌쨋처횁체 
					// 쩍쨘횇챈 쩔��쨘챗 횁짝쨍짰쨍쨋 쩐첩쩐횠쨈횢.
					ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
					return;
				}

				iValue++;
				if (iValue > 7)
					iValue = 7;
				else {
					// 쩐첨짹횞쨌쨔��횑쨉책 쩌쨘째첩. 쩐횈��횑횇횤 횈짱쩌쨘 쨔횢짼횢째챠
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF; // 쨘챰횈짰 횇짭쨍짰쩐챤 
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28); // 쩐첨짹횞쨌쨔��횑쨉책쨉횊 쨘챰횈짰째짧 ��횚쨌횂	
					// 쩍쨘횇챈 쩔��쨘챗 횁짝쨍짰쨍쨋 쩐첩쩐횠쨈횢.
					ItemDepleteHandler(iClientH, iSoxH, FALSE, TRUE);
				}
			}

			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);

			break;
		}
		break;

	default:
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
		break;
	}
}

int CGame::HeroItemChecker(int iItemID, short sEnemyKill, char cContribution, char cSide)
{

	switch (iItemID) {

	case 400: // AresdenHeroCape 
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 0;
		if (cSide != 0) cSide = 1;
		break;

	case 401: // ElvineHeroCape 
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 0;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroHelm
	case 403: // aHeroHelm(M)
	case 404: // aHeroHelm(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 405: // aHeroHelm(M)
	case 406: // aHeroHelm(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroCap
	case 407: // aHeroCap(M)
	case 408: // aHeroCap(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 409: // aHeroCap(M)
	case 410: // aHeroCap(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroArmor
	case 411: // eHeroArmor(M)
	case 412: // eHeroArmor(W)
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 30;
		if (cSide != 0) cSide = 1;
		break;
	case 413: // eHeroArmor(M)
	case 414: // eHeroArmor(W)
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 30;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroRobe
	case 415: // aHeroRobe(M)
	case 416: // aHeroRobe(W)
		if (sEnemyKill != 0) sEnemyKill = 200;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 417: // aHeroRobe(M)
	case 418: // aHeroRobe(W)
		if (sEnemyKill != 0) sEnemyKill = 200;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroHauberk
	case 419: // aHeroHauberk(M)
	case 420: // aHeroHauberk(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 10;
		if (cSide != 0) cSide = 1;
		break;
	case 421: // eHeroHauberk(M)
	case 422: // eHeroHauberk(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 10;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroLeggings
	case 423: // aHeroLeggings(M)
	case 424: // aHeroLeggings(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 15;
		if (cSide != 0) cSide = 1;
		break;
	case 425: // eHeroLeggings(M)
	case 426: // eHeroLeggings(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 15;
		if (cSide != 0) cSide = 2;
		break;

	case 427: // AresdenHeroCape+1 
		if (cSide != 0) cSide = 1;
		break;

	case 428: // ElvineHeroCape+1
		if (cSide != 0) cSide = 2;
		break;

	default:
		return 0;
	}
	return iItemID;

}

BOOL CGame::iUpgradeHeroCapeRequirements(int iClientH, int iItemIndex)
{
	int iAfterItemID, iRequiredEnemyKills, iRequiredContribution, iStoneNumber, i;
	int iBeforeItemID;

	iAfterItemID = 0;
	iRequiredEnemyKills = 30;
	iRequiredContribution = 50;
	iStoneNumber = 0;
	i = 0;
	iBeforeItemID = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum;
	if (iBeforeItemID == 400) {
		_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)m_pClientList[iClientH]->m_pItemList[iItemIndex], FALSE);
		iAfterItemID = 427;
		iStoneNumber = 657;
	}
	else if (iBeforeItemID == 401) {
		_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)m_pClientList[iClientH]->m_pItemList[iItemIndex], FALSE);
		iAfterItemID = 428;
		iStoneNumber = 657;
	}
	else {
		return FALSE;
	}
	if (iRequiredEnemyKills > m_pClientList[iClientH]->m_iEnemyKillCount) return FALSE;
	if (iRequiredContribution > m_pClientList[iClientH]->m_iContribution) return FALSE;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == iStoneNumber)) break;

	if ((i == DEF_MAXITEMS) || (iStoneNumber == 0)) return FALSE;
	if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], iAfterItemID) == FALSE) return FALSE;
	m_pClientList[iClientH]->m_iEnemyKillCount -= iRequiredEnemyKills;
	m_pClientList[iClientH]->m_iContribution -= iRequiredContribution;
	if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
		ItemDepleteHandler(iClientH, i, FALSE, TRUE);
		return TRUE;
	}
	return FALSE;
}

//50Cent - Repair All
void CGame::RequestRepairAllItemsHandler(int iClientH)
{
	int i, price;
	double d1, d2, d3;
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	m_pClientList[iClientH]->totalItemRepair = 0;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
			if (((m_pClientList[iClientH]->m_pItemList[i]->m_cCategory >= 1) && (m_pClientList[iClientH]->m_pItemList[i]->m_cCategory <= 12)) ||
				((m_pClientList[iClientH]->m_pItemList[i]->m_cCategory >= 43) && (m_pClientList[iClientH]->m_pItemList[i]->m_cCategory <= 50)))
			{
				if (m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan == m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan)
					continue;
				if (m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan <= 0)
					price = (m_pClientList[iClientH]->m_pItemList[i]->m_wPrice / 2);
				else
				{
					d1 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan);
					if (m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan != 0)
						d2 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan);
					else
						d2 = 1.0f;
					d3 = ((d1 / d2) * 0.5f);
					d2 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wPrice);
					d3 = (d3 * d2);
					price = ((m_pClientList[iClientH]->m_pItemList[i]->m_wPrice / 2) - (short)(d3));
				}
				m_pClientList[iClientH]->m_stRepairAll[m_pClientList[iClientH]->totalItemRepair].index = i;
				m_pClientList[iClientH]->m_stRepairAll[m_pClientList[iClientH]->totalItemRepair].price = price;
				m_pClientList[iClientH]->totalItemRepair++;
			}
	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRALLPRICES, NULL, NULL, NULL, NULL);
}

void CGame::RequestRepairAllItemsDeleteHandler(int iClientH, int index)
{
	int i;
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	for (i = index; i < m_pClientList[iClientH]->totalItemRepair; i++)
		m_pClientList[iClientH]->m_stRepairAll[i] = m_pClientList[iClientH]->m_stRepairAll[i + 1];
	m_pClientList[iClientH]->totalItemRepair--;
	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRALLPRICES, NULL, NULL, NULL, NULL);
}

void CGame::RequestRepairAllItemsConfirmHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;
	int      iRet, i, totalPrice = 0;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

	for (i = 0; i < m_pClientList[iClientH]->totalItemRepair; i++)
		totalPrice += m_pClientList[iClientH]->m_stRepairAll[i].price;

	if (dwGetItemCount(iClientH, "Gold") < (DWORD)totalPrice)
	{
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_NOTENOUGHGOLD;
		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 0;
		cp++;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}

	}
	else
	{
		for (i = 0; i < m_pClientList[iClientH]->totalItemRepair; i++)
		{
			m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wMaxLifeSpan;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMREPAIRED, m_pClientList[iClientH]->m_stRepairAll[i].index, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wCurLifeSpan, NULL, NULL);
		}
		iCalcTotalWeight(SetItemCount(iClientH, "Gold", dwGetItemCount(iClientH, "Gold") - totalPrice));
	}
}


// MORLA 2.4 - Trade Items
void CGame::RequestPurchaseItemHandler2(int iClientH, char* pItemName, int iNum)
{
	class CItem* pItem;
	char* cp, cItemName[21], cData[100];
	short* sp;
	DWORD* dwp, dwItemCount;
	WORD* wp, wTempPrice;
	int   i, iRet, iEraseReq;
	int iEKCost = 0, iDPCost = 0;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;



	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cItemName, sizeof(cItemName));
	// MORLA 2.5 - Trade items x EKs
	if (memcmp(pItemName, "XelimaBlade", 11) == 0) { iDPCost = 0; iEKCost = 3500; }
	if (memcmp(pItemName, "XelimaAxe", 9) == 0) { iDPCost = 0; iEKCost = 4500; }
	if (memcmp(pItemName, "XelimaRapier", 12) == 0) { iDPCost = 0; iEKCost = 2000; }
	if (memcmp(pItemName, "KlonessBlade", 12) == 0) { iDPCost = 0; iEKCost = 3500; }
	if (memcmp(pItemName, "KlonessAxe", 10) == 0) { iDPCost = 0; iEKCost = 3500; }
	if (memcmp(pItemName, "KlonessEsterk", 13) == 0) { iDPCost = 0; iEKCost = 3000; }
	if (memcmp(pItemName, "BerserkWand(MS.20)", 18) == 0) { iDPCost = 0; iEKCost = 4500; }
	if (memcmp(pItemName, "KlonessWand(MS.20)", 18) == 0) { iDPCost = 0; iEKCost = 2000; }
	if (memcmp(pItemName, "ResurWand(MS.20)", 16) == 0) { iDPCost = 0; iEKCost = 2000; }
	if (memcmp(pItemName, "I.M.CManual", 11) == 0) { iDPCost = 0; iEKCost = 3000; }
	if (memcmp(pItemName, "MerienShield", 12) == 0) { iDPCost = 0; iEKCost = 6000; }
	if (memcmp(pItemName, "MerienPlateMailM", 16) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "MerienPlateMailW", 16) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "SwordofMedusa", 13) == 0) { iDPCost = 0; iEKCost = 5000; }
	if (memcmp(pItemName, "DemonSlayer", 11) == 0) { iDPCost = 0; iEKCost = 1000; }
	if (memcmp(pItemName, "DarkElfBow", 10) == 0) { iDPCost = 0; iEKCost = 500; }
	if (memcmp(pItemName, "GiantBattleHammer", 17) == 0) { iDPCost = 0; iEKCost = 6000; }
	if (memcmp(pItemName, "StormBringer", 12) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "DarkExecutor", 12) == 0) { iDPCost = 0; iEKCost = 4500; }
	if (memcmp(pItemName, "LightingBlade", 13) == 0) { iDPCost = 0; iEKCost = 4500; }
	if (memcmp(pItemName, "RingoftheAbaddon", 16) == 0) { iDPCost = 0; iEKCost = 2500; }
	if (memcmp(pItemName, "RingofArcmage", 13) == 0) { iDPCost = 0; iEKCost = 2500; }
	if (memcmp(pItemName, "NecklaceOfMerien", 16) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "NecklaceOfXelima", 16) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "KnecklaceOfMedusa", 17) == 0) { iDPCost = 0; iEKCost = 5000; }
	if (memcmp(pItemName, "KnecklaceOfIceEle", 17) == 0) { iDPCost = 0; iEKCost = 2000; }
	if (memcmp(pItemName, "HeroNeckMage(MD+35)", 19) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "HeroNeckWar(DM+35)", 18) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "RingofHeroMage", 14) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "RingOfHeroWar", 13) == 0) { iDPCost = 0; iEKCost = 4000; }
	if (memcmp(pItemName, "HeroWand", 8) == 0) { iDPCost = 0; iEKCost = 4000; }

	// MORLA 2.5 - Trade items x DeathMach Points
	if (memcmp(pItemName, "ZemstoneofSacrifice", 19) == 0) { iDPCost = 3; iEKCost = 0; }
	if (memcmp(pItemName, "StoneOfXelima", 13) == 0) { iDPCost = 4; iEKCost = 0; }
	if (memcmp(pItemName, "StoneOfMerien", 13) == 0) { iDPCost = 4; iEKCost = 0; }
	if (memcmp(pItemName, "RingofDemonpower", 16) == 0) { iDPCost = 10; iEKCost = 0; }
	if (memcmp(pItemName, "RingofGrandMage", 15) == 0) { iDPCost = 10; iEKCost = 0; }
	if (memcmp(pItemName, "MagicWand(MS30-LLF)", 18) == 0) { iDPCost = 12; iEKCost = 0; }
	if (memcmp(pItemName, "HuntSword", 9) == 0) { iDPCost = 100; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorHauberk", 16) == 0) { iDPCost = 50; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorBerkW", 14) == 0) { iDPCost = 50; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorHelmM", 14) == 0) { iDPCost = 60; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorHelmW", 14) == 0) { iDPCost = 60; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorHatM", 13) == 0) { iDPCost = 30; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorHatW", 13) == 0) { iDPCost = 30; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorLeggs", 14) == 0) { iDPCost = 60; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorLeggsW", 15) == 0) { iDPCost = 60; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorMailM", 14) == 0) { iDPCost = 70; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorMailW", 14) == 0) { iDPCost = 70; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorAxe", 12) == 0) { iDPCost = 80; iEKCost = 0; }
	if (memcmp(pItemName, "GladeitorWandMS70", 17) == 0) { iDPCost = 40; iEKCost = 0; }

	if ((m_pClientList[iClientH]->m_iEnemyKillCount < iEKCost)
		|| (m_pClientList[iClientH]->m_iDGPoints < iDPCost)) return;

	if ((iDPCost == 0) && (iEKCost == 0)) return;

	memcpy(cItemName, pItemName, 20);
	dwItemCount = 1;

	for (i = 1; i <= iNum; i++) {

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == FALSE) {
			delete pItem;
		}
		else {

			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_ITEMTRADE;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				*cp = 1;
				cp++;

				memcpy(cp, pItem->m_cName, 20);
				cp += 20;

				dwp = (DWORD*)cp;
				*dwp = pItem->m_dwCount;
				cp += 4;

				*cp = pItem->m_cItemType;
				cp++;

				*cp = pItem->m_cEquipPos;
				cp++;

				*cp = (char)0;
				cp++;

				sp = (short*)cp;
				*sp = pItem->m_sLevelLimit;
				cp += 2;

				*cp = pItem->m_cGenderLimit;
				cp++;

				wp = (WORD*)cp;
				*wp = pItem->m_wCurLifeSpan;
				cp += 2;

				wp = (WORD*)cp;
				*wp = pItem->m_wWeight;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSprite;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSpriteFrame;
				cp += 2;

				*cp = pItem->m_cItemColor;
				cp++;

				wp = (WORD*)cp;
				*wp = iEKCost;
				wTempPrice = iEKCost;
				cp += 2;

				if (iEraseReq == 1) delete pItem;


				m_pClientList[iClientH]->m_iEnemyKillCount -= iEKCost;
				m_pClientList[iClientH]->m_iDGPoints -= iDPCost;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, NULL, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPDGDEATHS, m_pClientList[iClientH]->m_iDGPoints, m_pClientList[iClientH]->m_iTotalDGKills, m_pClientList[iClientH]->m_iRating, NULL);

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 48);

				iCalcTotalWeight(iClientH);

				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
			}
			else
			{
				delete pItem;

				iCalcTotalWeight(iClientH);

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
			}
		}
	}
}

//dkset by diuude and update by drawjer and modified by Ovl
void CGame::GetDkSet(int iClientH)
{
	class CItem* pItem;
	int dkM[] = { 706,707,708,710 };
	int dkW[] = { 724,725,726,728 };
	int dmM[] = { 711,712,713,913 };
	int dmW[] = { 729,730,731,914 };
	int weapon, iItemID, i, iEraseReq;
	char cTxt[120];
	DWORD  dwGoldCount = dwGetItemCount(iClientH, "Gold");

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_iLevel < m_iPlayerMaxLevel) return;
	if (dwGoldCount < m_iDKCost) 
	{
		wsprintf(cTxt, "Not enough gold. You need %d gold.", m_iDKCost);
		SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, cTxt);
		return; // centu - el dk set cuesta gold
	}
	if (((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "cityhall_1") == 0) || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "cityhall_2") == 0))) {
		for (i = 0; i <= 4; i++)
		{
			pItem = new class CItem;
			iItemID = -1;
			if (m_pClientList[iClientH]->m_cSex == 1)
			{
				if (m_pClientList[iClientH]->m_iStr > m_pClientList[iClientH]->m_iInt)
				{
					iItemID = dkM[i];
				}
				else
				{
					iItemID = dmM[i];
				}
			}
			else
			{
				if (m_pClientList[iClientH]->m_iStr > m_pClientList[iClientH]->m_iInt)
				{
					iItemID = dkW[i];
				}
				else
				{
					iItemID = dmW[i];
				}
			}
			if (i == 4)
			{
				if (m_pClientList[iClientH]->m_iStr > m_pClientList[iClientH]->m_iInt)
				{
					weapon = 709;
				}
				else
				{
					weapon = 714;
				}
				iItemID = weapon;
			}
			SetItemCount(iClientH, "Gold", (dwGoldCount - m_iDKCost));
			if (iItemID != -1) //if any error occures, dont crash character
			{
				_bInitItemAttr(pItem, iItemID);
				pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				_bAddClientItemList(iClientH, pItem, &iEraseReq);
				SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, NULL);
			}
		}
	}
}

int CGame::_iGetArrowItemIndex(int iClientH)
{
	int i;

	if (m_pClientList[iClientH] == NULL) return -1;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {

			// Arrow 쩐횈��횑횇횤��횑 1째쨀 ��횑쨩처 ��횜��쨍쨍챕 ��횓쨉짝쩍쨘 째짧��쨩 쨔횦횊짱횉횗쨈횢.
			if ((m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_ARROW) &&
				(m_pClientList[iClientH]->m_pItemList[i]->m_dwCount > 0))
				return i;
		}

	return -1;
}

void CGame::UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID)
{
	int iMax, iV1, iV2, iV3, iSEV1, iEffectResult = 0;
	DWORD dwTime, dwGUID;
	short sTemp, sTmpType, sTmpAppr1;
	char cSlateType[20];
	BOOL bDepleteNow = TRUE;

	dwTime = timeGetTime();
	ZeroMemory(cSlateType, sizeof(cSlateType));

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;


	if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT)) {

		// 쩐횈��횑횇횤��횉 횊쩔째첬쩔징 쨍횂쨈횂 횄쨀쨍짰쨍짝 횉횗쨈횢. 
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
		case DEF_ITEMEFFECTTYPE_WARM: // v2.172 2002-7-5 해동 시약. 

			// 냉동 상태인 경우 해동 되었다는 메세지를 보내준다. 
			if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 1) {

				bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_ICE);

				// 효과가 해제될 때 발생할 딜레이 이벤트를 등록한다.
				bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (1 * 1000),
					iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);

			}

			m_pClientList[iClientH]->m_dwWarmEffectTime = dwTime;
			break;

			// Pandora's Box By Diuuude
		case DEF_ITEMEFFECTTYPE_PANDORA:
			int iProb, iResult1, iResult2, iPandoraItem, iEraseReq, iDamage, iDecRep, iLockTime;
			class CItem* pItem;
			/* First off all, we have to select a random event for opening box
			Effects can be :
			- OK : Restart level's Exp
			- OK : Get an Item
			- OK : Loose Rep (5 -> 50)
			- OK : Go to jail (1 to 5 mins)
			- OK : Loose HP
			- OK : Loose MP
			- OK : Loose SP
			- OK : Get some MJ Points

			*/
			iResult1 = iDice(1, 8);
			switch (iResult1) {
			case 1: // All the exp you gained this level is reseted

				m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel];
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's Box reseted your Exp...");
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);
				break;
			case 2: // You get a random item depending on your rep
				// Check user's Rep to set a prob depending on his reps
				if (m_pClientList[iClientH]->m_iRating < -50) iProb = 500;
				if ((m_pClientList[iClientH]->m_iRating >= -50) && (m_pClientList[iClientH]->m_iRating < 0)) iProb = 400;
				if (m_pClientList[iClientH]->m_iRating == 0) iProb = 350;
				if ((m_pClientList[iClientH]->m_iRating > 0) && (m_pClientList[iClientH]->m_iRating <= 50)) iProb = 300;
				if ((m_pClientList[iClientH]->m_iRating > 50) && (m_pClientList[iClientH]->m_iRating <= 100)) iProb = 250;
				if ((m_pClientList[iClientH]->m_iRating > 100) && (m_pClientList[iClientH]->m_iRating <= 200)) iProb = 200;
				if (m_pClientList[iClientH]->m_iRating > 200) {
					iProb = 150 - (m_pClientList[iClientH]->m_iRating / 10);
				}
				iResult2 = iDice(1, iProb);
				switch (iResult2) {
					// Good Items
				case 1:
					iPandoraItem = 308; // Magic Necklace (MS10)
					break;
				case 2:
					iPandoraItem = 300; // Magic Necklace (RM10)
					break;
				case 3:
					iPandoraItem = 275; // Ogre Summon Potion
					break;
				case 4:
					iPandoraItem = 311; // Magic Necklace (DF+10)
					break;
				case 5:
					iPandoraItem = 338; // Memorial Ring
					break;
				case 6:
					iPandoraItem = 380; // Ice Storm Manual
					break;
				case 7:
					iPandoraItem = 381; // Mass Fire Strike Manual
					break;
				case 8:
					iPandoraItem = 490; // Blood Sword
					break;
				case 9:
					iPandoraItem = 618; // Dark-Elf Bow
					break;
				case 10:
					iPandoraItem = 620; // Merien Shield
					break;
				case 11:
					iPandoraItem = 636; // Ring Of Grand Mage
					break;
				case 12:
					iPandoraItem = 641; // Necklace Of Medusa
					break;
				case 13:
					iPandoraItem = 648; // Necklace Of Liche
					break;
				case 14:
					iPandoraItem = 765; // Third Memorial Ring
					break;
				case 15:
					iPandoraItem = 849; // Kloness Blade
					break;
				case 16:
					iPandoraItem = 862; // Berserk Wand (MS.10)
					break;
				case 17:
					iPandoraItem = 864; // Kloness Wand (MS.10)
					break;
				case 18:
					iPandoraItem = 866; // Resur Wand (MS.10)
					break;
				case 19:
					iPandoraItem = 291; // MagicWand (MS30-LLF)
					break;
				case 20:
					iPandoraItem = 290; // Flameberge +3 (LLF)
					break;
				case 21:
					iPandoraItem = 292; // Golden Axe (LLF)
					break;
				case 22:
					iPandoraItem = 491; // Blood Axe
					break;
				case 23:
					iPandoraItem = 492; // Blood Rapier
					break;
				case 24:
					iPandoraItem = 610; // Xelima Blade
					break;
				case 25:
					iPandoraItem = 611; // Xelima Axe
					break;
				case 26:
					iPandoraItem = 612; // Xelima Rapier
					break;
				case 27:
					iPandoraItem = 613; // Sword of Medusa
					break;
				case 28:
					iPandoraItem = 614; // Sword of Ice Elemental
					break;
				case 29:
					iPandoraItem = 616; // Demon Slayer
					break;
				case 30:
					iPandoraItem = 630; // Ring of the Xelima
					break;
				case 31:
					iPandoraItem = 633; // Ring of Demon Power
					break;
				case 32:
					iPandoraItem = 635; // Ring of Mage
					break;
				case 33:
					iPandoraItem = 636; // Ring of Arc Mage
					break;
				case 34:
					iPandoraItem = 734; // Ring of Dragon power
					break;
				case 35:
					iPandoraItem = 735; // Ring of Demon Power
					break;
				case 36:
					iPandoraItem = 844; // Black Shadow Sword
					break;
				case 37:
					iPandoraItem = 847; // Dark Executor
					break;
				case 38:
					iPandoraItem = 852; // Cancel Manual
					break;
				case 39:
					iPandoraItem = 853; // Earth Shock Wave Manual
					break;
				case 40:
					iPandoraItem = 854; // Inhibition Magic Casting Manual
					break;
				case 41:
					iPandoraItem = 858; // Necklace Of Merien
					break;
				case 42:
					iPandoraItem = 859; // Necklace Of Kloness
					break;
				case 43:
					iPandoraItem = 860; // Necklace Of Xelima
					break;
					/* Weak Items: if you add items, be sure that the weak items cases # are greater
					 than the good item cases #. It will allow players with high rating
					 to get better items.*/
				case 59:
					iPandoraItem = 4; // Dagger+1
					break;
				case 60:
					iPandoraItem = 46; // Claymore
					break;
				case 61:
					iPandoraItem = 75; // Short Bow
					break;
				case 62:
					iPandoraItem = 91; // Red Potion
					break;
				case 63:
					iPandoraItem = 93; // Blue Potion
					break;
				case 64:
					iPandoraItem = 95; // Green Potion
					break;
				case 65:
					iPandoraItem = 100; // Fish
					break;
				case 66:
					iPandoraItem = 402; // Cape
					break;
				case 67:
					iPandoraItem = 522; // Flower Pot
					break;
				case 68:
					iPandoraItem = 824; // Carrot
					break;
				case 69:
					iPandoraItem = 883; // ArmorDye(Gold)
					break;
				default:
					iPandoraItem = 0; // Nothing
					break;
				}
				if (iPandoraItem != 0) {
					pItem = new class CItem;
					_bInitItemAttr(pItem, iPandoraItem);
					_bAddClientItemList(iClientH, pItem, &iEraseReq);
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Yeah, Pandora's box had an item inside !!");
					SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, NULL);
				}
				else {
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's box was empty !!");
				}

				break;
			case 3: // Pandora's Box Explodes. You loose HP depending on reps
				iResult2 = iDice(2, 3);
				iDamage = (iResult2 * 100) - m_pClientList[iClientH]->m_iRating;
				m_pClientList[iClientH]->m_iLastDamage = iDamage;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's box exploded !!");
				SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);
				break;
			case 4: // Pandora's Box decreases your Reps
				iResult2 = iDice(1, 10);
				iDecRep = (iResult2 * 5);
				m_pClientList[iClientH]->m_iRating -= iDecRep;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's box decreased your Reputation !!");
				break;
			case 5: // Pandora's Box decreases your MP
				m_pClientList[iClientH]->m_iMP = 0;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, NULL, NULL, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's box decreased your Mana !!");
				break;
			case 6: // Pandora's Box decreases your SP
				m_pClientList[iClientH]->m_iSP = 0;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Doh, Pandora's box decreased your Stamina !!");
				break;
			case 7: // Pandora's Box Gives you some MJ Points
				iResult2 = iDice(1, 5);
				m_pClientList[iClientH]->m_iGizonItemUpgradeLeft += iResult2;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "Yeah, Your Majestic Points have been increased by Pandora's Box !!");
				break;
			case 8: // Pandora's Box send you in jail
				iResult2 = iDice(1, 5);
				iLockTime = (iResult2 * 60);
				switch (m_pClientList[iClientH]->m_cSide) {
				case 1:
					ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
					strcpy(m_pClientList[iClientH]->m_cLockedMapName, "arejail");
					RequestTeleportHandler(iClientH, "2   ", "arejail", -1, -1);
					break;
				case 2:
					ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
					strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");;
					RequestTeleportHandler(iClientH, "2   ", "elvjail", -1, -1);
					break;
				}
				m_pClientList[iClientH]->m_iLockedMapTime = iLockTime;
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = iLockTime;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, "You were busted for using illegal Pandora's Box");
				break;
			}
			break;

		case DEF_ITEMEFFECTTYPE_LOTTERY:

			break;

		case DEF_ITEMEFFECTTYPE_SLATES:
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) {
				// Full Ancient Slate ??
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 867) {
					// Slates dont work on Heldenian Map
					switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2) {
					case 2: // Bezerk slate
						m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = TRUE;
						SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
						bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (1000 * 600),
							iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_BERSERK, 1, NULL, NULL);
						strcpy(cSlateType, "Berserk");
						break;

					case 1: // Invincible slate
						if (strlen(cSlateType) == 0) {
							strcpy(cSlateType, "Invincible");
						}
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_INVINCIBLE, NULL, NULL, NULL, NULL);
						break;
					case 3: // Mana slate
						if (strlen(cSlateType) == 0) {
							strcpy(cSlateType, "Mana");
						}
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_MANA, NULL, NULL, NULL, NULL);
						break;
					case 4: // Exp slate
						if (strlen(cSlateType) == 0) {
							strcpy(cSlateType, "Exp");
						}
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_EXP, NULL, NULL, NULL, NULL);
						break;
					}
					SetSlateFlag(iClientH, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2, TRUE);
					bRegisterDelayEvent(DEF_DELAYEVENTTYPE_ANCIENT_TABLET, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2,
						dwTime + (1000 * 600), iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);

				}
				if (strlen(cSlateType) > 0)
					_bItemLog(DEF_ITEMLOG_USE, iClientH, strlen(cSlateType), m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			}
			break;

		case DEF_ITEMEFFECTTYPE_HP:
			iMax = iGetMaxHP(iClientH);
			if (m_pClientList[iClientH]->m_iHP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iHP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iHP > iMax) m_pClientList[iClientH]->m_iHP = iMax;
				if (m_pClientList[iClientH]->m_iHP <= 0)   m_pClientList[iClientH]->m_iHP = 1;

				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
			}
			break;

		case DEF_ITEMEFFECTTYPE_MP:
			iMax = iGetMaxMP(iClientH);

			if (m_pClientList[iClientH]->m_iMP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else
				{
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iMP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iMP > iMax)
					m_pClientList[iClientH]->m_iMP = iMax;

				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, NULL, NULL, NULL, NULL);
			}
			break;

		case DEF_ITEMEFFECTTYPE_SP:
			iMax = iGetMaxSP(iClientH);

			if (m_pClientList[iClientH]->m_iSP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iSP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iSP > iMax)
					m_pClientList[iClientH]->m_iSP = iMax;

				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
			}

			if (m_pClientList[iClientH]->m_bIsPoisoned == TRUE) {
				// 횁횩쨉쨋쨉횊 쨩처횇횂쩔쨈쨈횢쨍챕 횁횩쨉쨋��쨩 횉짭쨈횢.
				m_pClientList[iClientH]->m_bIsPoisoned = FALSE;
				// 횁횩쨉쨋��횑 횉짰쨌횊��쩍��쨩 쩐횏쨍째쨈횢. 
				SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE); // removes poison aura when using a revitalizing potion
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, NULL, NULL, NULL);
			}
			break;

		case DEF_ITEMEFFECTTYPE_HPSTOCK:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
			iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;

			m_pClientList[iClientH]->m_iHPstock += iDice(iV1, iV2) + iV3;
			if (m_pClientList[iClientH]->m_iHPstock < 0)   m_pClientList[iClientH]->m_iHPstock = 0;
			if (m_pClientList[iClientH]->m_iHPstock > 500) m_pClientList[iClientH]->m_iHPstock = 500;

			// 쨔챔째챠횉횆��쨩 횉횠째찼횉횗쨈횢. 
			m_pClientList[iClientH]->m_iHungerStatus += iDice(iV1, iV2) + iV3;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HUNGER, m_pClientList[iClientH]->m_iHungerStatus, NULL, NULL, NULL); // MORLA2 - Muestra nuevamente
			if (m_pClientList[iClientH]->m_iHungerStatus > 100) m_pClientList[iClientH]->m_iHungerStatus = 100;
			if (m_pClientList[iClientH]->m_iHungerStatus < 0)   m_pClientList[iClientH]->m_iHungerStatus = 0;
			break;


		case DEF_ITEMEFFECTTYPE_STUDYSKILL: //Manual
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
			iSEV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
			if (iSEV1 == 0) 
			{
				TrainSkillResponse(TRUE, iClientH, iV1, iV2);
			}
			else 
			{
				TrainSkillResponse(TRUE, iClientH, iV1, iSEV1);
			}
			break;

		case DEF_ITEMEFFECTTYPE_STUDYMAGIC:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			if (m_pMagicConfigList[iV1] != NULL)
				RequestStudyMagicHandler(iClientH, m_pMagicConfigList[iV1]->m_cName, bDepleteNow, FALSE);
			break;

			// New 15/05/2004 Changed
		case DEF_ITEMEFFECTTYPE_MAGIC:
			// 횇천쨍챠 쨍챨쨉책쩔쨈쨈횢쨍챕 쨍쨋쨔첵 횊쩔째첬 쩐횈��횑횇횤 쨩챌쩔챘쩍횄쩔징 횉횠횁짝쨉횊쨈횢.

			if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
				SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);

				bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
				m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
			}


			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
			case 1:
				// Recall 쨍쨋쨔첵 횊쩔째첬째징 ��횜쨈횂 쩐횈��횑횇횤. 
				// testcode
				RequestTeleportHandler(iClientH, "1  ");
				break;

			case 2:
				// 횇천쨍챠 쨍쨋쨔첵횊쩔째첬째징 ��횜쨈횂 쩐횈��횑횇횤. 
				PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 32, TRUE);
				break;

			case 3:
				// 횇천쨍챠 쨍쨋쨔첵 횄짙짹창. 쨩챌횇천��책 쨀쨩쨘횓쨍챕 쩌횘쩔챘쩐첩쨈횢. 
				if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == FALSE)
					PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 34, TRUE);
				break;

			case 4:
				// fixed location teleportation: ��횚��책짹횉 쨉챤쨉챤
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) {
				case 1:
					
						// 쨘챠쨍짰쨉첫 쩐횈��횕쨌횓 째짙쨈횢 
						if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) != 0) {
							//v1.42
							ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
							RequestTeleportHandler(iClientH, "2   ", "bisle", -1, -1);
						}
					
					break;

				//LifeX Added Reset Stat Scroll
				case 9:
					m_pClientList[iClientH]->m_iMag;
					m_pClientList[iClientH]->m_iMag = 10;
					m_pClientList[iClientH]->m_iStr;
					m_pClientList[iClientH]->m_iStr = 10;
					m_pClientList[iClientH]->m_iDex;
					m_pClientList[iClientH]->m_iDex = 10;
					m_pClientList[iClientH]->m_iCharisma;
					m_pClientList[iClientH]->m_iCharisma = 10;
					m_pClientList[iClientH]->m_iInt;
					m_pClientList[iClientH]->m_iInt = 10;
					m_pClientList[iClientH]->m_iVit;
					m_pClientList[iClientH]->m_iVit = 10;

					m_pClientList[iClientH]->m_iLU_Pool = 1432;
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND], FALSE);
					ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND], FALSE);
					ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
					bCheckMagicInt(iClientH);
					DeleteClient(iClientH, TRUE, TRUE);
					break;

				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					// 째찼횇천��책��쨍쨌횓 째짙쨈횢. 
					SYSTEMTIME SysTime;

					GetLocalTime(&SysTime);
					// v1.4311-3 쨘짱째챈 ��횚��책짹횉 횄쩌횇짤 쨈횧/쨀짱횂짜/쩍횄째짙��쨍쨌횓 횄쩌횇짤횉횗쨈횢. 
					// ��횚��책 째징쨈횋횉횗 쩍횄째짙쨘쨍쨈횢 ��횤째횇쨀짧 째째��쨍쨍챕 ��횚��책짹횉��횑 쨩챌쨋처횁첩쨈횢.
					if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue1 == SysTime.wMonth) ||
						(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue2 == SysTime.wDay) ||
						(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue3 > SysTime.wHour)) {
						// 쨀짱횂짜째징 횁짚횊짰횉횕횁철 쩐횎쨈횢. 쩐횈쨔짬쨌짹 횊쩔째첬째징 쩐첩째챠 ��횚��책짹횉��쨘 쨩챌쨋처횁첩쨈횢.

						char cDestMapName[11];
						ZeroMemory(cDestMapName, sizeof(cDestMapName));
						wsprintf(cDestMapName, "fightzone%d", m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 - 10);
						if (memcmp(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10) != 0) {
							//v1.42
							ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
							RequestTeleportHandler(iClientH, "2   ", cDestMapName, -1, -1);
						}
					}
					break;
				}
				break;

			case 5: // Heldenians scroll must be defined as DEF_ITEMTYPE_USE_PERM
					// Only Helnenian scrolls can be defined as DEF_ITEMTYPE_USE_PERM 
					// Scrolls not belonging to the user are depleted when entering function 
				if ((m_bIsHeldenianMode == TRUE)
					&& (m_bHeldenianWarInitiated == TRUE)
					&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0)
					&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1)
					&& (strcmp(m_pClientList[iClientH]->m_cMapName, "GodH") != 0))
				{
					memcpy(&dwGUID, &m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1, 4);
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER)
					{
						if (dwGUID == m_dwHeldenianGUID)
						{	// Scrolls from current Heldenian can summon
							if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6 == -1)// -1 allows using with weapon shield equiped
								PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, TRUE,
									m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
							else
								PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, TRUE,
									m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 * -1);
							m_pClientList[iClientH]->m_iWarContribution += 300;

						}
						// But older scrolls will be depleted without effect!
						ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
					}
					else
					{	// Not personal scrolls can summon and deplete scroll now
						if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6 == -1)// -1 allows using with weapon shield equiped
							PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, TRUE,
								m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
						else
							PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, TRUE,
								m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 * -1);
						ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
					}
				}
				else // No permanent scroll outside Special Heldenian summon scrolls
				{
					if (m_bIsHeldenianMode == FALSE) ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
				}
				break;
			}
			break;

		case DEF_ITEMEFFECTTYPE_FIRMSTAMINAR:
			m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			break;

		case DEF_ITEMEFFECTTYPE_CHANGEATTR:
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
			case 1:
				// 쨍횙쨍짰 쨩철��쨩 쨔횢짼횤쨈횢. 
				m_pClientList[iClientH]->m_cHairColor++;
				if (m_pClientList[iClientH]->m_cHairColor > 15) m_pClientList[iClientH]->m_cHairColor = 0;

				sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
				m_pClientList[iClientH]->m_sAppr1 = sTemp;
				break;

			case 2:
				// 쨍횙쨍짰 쩍쨘횇쨍��횕��쨩 쨔횢짼횤쨈횢.
				m_pClientList[iClientH]->m_cHairStyle++;
				if (m_pClientList[iClientH]->m_cHairStyle > 7) m_pClientList[iClientH]->m_cHairStyle = 0;

				sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
				m_pClientList[iClientH]->m_sAppr1 = sTemp;
				break;

			case 3:
				// 횉횉쨘횓쨩철��쨩 쨔횢짼횤쨈횢.
				// 쨀쨩쩔챘��쨩 쨔횢횇횁��쨍쨌횓 Appearance쨍짝 째챔쨩챗, 횉횘쨈챌횉횗쨈횢.
				m_pClientList[iClientH]->m_cSkin++;
				if (m_pClientList[iClientH]->m_cSkin > 3)
					m_pClientList[iClientH]->m_cSkin = 1;

				if (m_pClientList[iClientH]->m_cSex == 1)      sTemp = 1;
				else if (m_pClientList[iClientH]->m_cSex == 2) sTemp = 4;

				switch (m_pClientList[iClientH]->m_cSkin) {
				case 2:	sTemp++; break;
				case 3:	sTemp += 2; break;
				}
				m_pClientList[iClientH]->m_sType = sTemp;
				break;

			case 4:
				// 쩌쨘��체횊짱 - 쨍쨍쩐횪 쩔횎��쨩 ��횚째챠 ��횜쨈횢쨍챕 쩍횉횈횖횉횗쨈횢. 
				sTemp = m_pClientList[iClientH]->m_sAppr3 & 0xFF0F;
				if (sTemp == 0) {
					// sTemp째징 0��횑 쩐횈쨈횕쨋처쨍챕 째횗쩔횎, 쩌횙쩔횎, 쨔횢횁철횁횩 횉횗째징횁철쨍짝 ��횚째챠 ��횜쨈횂 째횒��횑쨈횢. 쩌쨘��체횊짱��쨩 횉횘 쩌철 쩐첩쨈횢. 
					if (m_pClientList[iClientH]->m_cSex == 1)
						m_pClientList[iClientH]->m_cSex = 2;
					else m_pClientList[iClientH]->m_cSex = 1;

					// 쨀쨩쩔챘��쨩 쨔횢횇횁��쨍쨌횓 Appearance쨍짝 째챔쨩챗, 횉횘쨈챌횉횗쨈횢.
					if (m_pClientList[iClientH]->m_cSex == 1) {
						// 쨀짼��횣��횑쨈횢. 
						sTmpType = 1;
					}
					else if (m_pClientList[iClientH]->m_cSex == 2) {
						// 쩔짤��횣��횑쨈횢.
						sTmpType = 4;
					}

					switch (m_pClientList[iClientH]->m_cSkin) {
					case 1:
						// 쨔챕��횓��횑쨍챕 짹횞쨈챘쨌횓.
						break;
					case 2:
						sTmpType++;
						break;
					case 3:
						sTmpType += 2;
						break;
					}

					sTmpAppr1 = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
					m_pClientList[iClientH]->m_sType = sTmpType;
					m_pClientList[iClientH]->m_sAppr1 = sTmpAppr1;
					//
				}
				break;
			}

			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
			break;
		}
		// *** Request Teleport Handler째징 ��횤쨉쩔쨉횉쨍챕 ��횑쨔횑 쨉짜��횑횇횒 ��첬��책��횑 쩔채횄쨩쨉횊 쨩처횇횂��횑쨔횉쨌횓 ��횑횊횆쩔징 쩐횈��횑횇횤��쨩 쩐첩쩐횜쨘횁쩐횩 쩌횘쩔챘��횑 쩐첩쨈횢. 
		// 쩐횈��횑횇횤��쨩 쨍횛��첬 쩐첩쩐횠쨈횢.
		ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST) {
		// 쨩챌쩔챘횉횕쨍챕쩌짯 쨍챰횉짜횁철횁징��쨩 횁철횁짚횉횕쨈횂 쩐횈��횑횇횤.
		// dX, dY��횉 횁횂횉짜째징 ��짱횊쩔 쨔체��짠 쨀쨩쩔징 ��횜쨈횂횁철 횊짰��횓횉횠쩐횩 횉횗쨈횢.
		if (_bDepleteDestTypeItemUseEffect(iClientH, dX, dY, sItemIndex, sDestItemID) == TRUE)
			ItemDepleteHandler(iClientH, sItemIndex, TRUE, TRUE);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) {
		// 횊짯쨩챙��쨩 횉횘쨈챌횉횗쨈횢. 
		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_PERM) {
		// 쩔쨉짹쨍횊첨 쩐쨉 쩌철 ��횜쨈횂 쩐횈��횑횇횤. 횁챦 쩐짼째챠쨀짧쨉쨉 쩐첩쩐챤횁철횁철 쩐횎쨈횂 쩐횈��횑횇횤. (ex: 횁철쨉쨉) 
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
		case DEF_ITEMEFFECTTYPE_SHOWLOCATION:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			switch (iV1) {
			case 1:
				// 횉철��챌 ��횣쩍횇��횉 ��짠횆징쨍짝 쨘쨍쩔짤횁횠쨈횢. 
				if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 1, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 2, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "middleland") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 3, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 4, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone2") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 5, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone1") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 6, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone4") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 7, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone3") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 8, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "arefarm") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 9, NULL, NULL);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvfarm") == 0)
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 10, NULL, NULL);
				else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 0, NULL, NULL);
				break;
			}
			break;
		}
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_SKILL) {
		// 짹창쩌첬째첬 째체쨌횄쨉횊 쩐횈��횑횇횤��쨩 쨩챌쩔챘횉횗쨈횢. 쩐횈��횑횇횤��횉 쩌철쨍챠��쨩 쨀쨌횄횩째챠 쨉척쨌쨔��횑 ��횑쨘짜횈짰쩔징 쨉챤쨌횕횉횗쨈횢. 

		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) ||
			(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) ||
			(m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] == TRUE)) {
			// 쩐횈��횑횇횤��횉 쩌철쨍챠��횑 쨈횢 횉횩째횇쨀짧 쩐첩째횇쨀짧 째체쨌횄 쩍쨘횇쨀��쨩 쨩챌쩔챘횁횩��횑쨋처쨍챕 쨔짬쩍횄 
			return;
		}
		else {
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan != 0) {
				// 횄횜쨈챘 쩌철쨍챠��횑 0��횑쨍챕 쨩챌쩔챘횉횠쨉쨉 쩌철쨍챠��횑 횁횢횁철 쩐횎쨈횂쨈횢.
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan--;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CURLIFESPAN, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) {
					// 쩐횈��횑횇횤��횉 쩌철쨍챠��횑 쨈횢 쨉횉쩐첬쨈횢.
					// 쩐횈��횑횇횤��횑 쨍횁째징횁쨀쨈횢쨈횂 쨍횧쩍횄횁철 <- ��횑째횋 쨔횧��쨍쨍챕 ��책횂첩횊짯쨍챕쩔징쩌짯 횉횠횁짝쩍횄횆횗쩐횩 횉횗쨈횢.
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMLIFESPANEND, DEF_EQUIPPOS_NONE, sItemIndex, NULL, NULL);
				}
				else {
					// 짹창쩌첬 쨩챌쩔챘 쩍횄째짙 ID째짧��쨩 짹쨍횉횗쨈횢. v1.12
					int iSkillUsingTimeID = (int)timeGetTime();

					bRegisterDelayEvent(DEF_DELAYEVENTTYPE_USEITEM_SKILL, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill,
						dwTime + m_pSkillConfigList[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill]->m_sValue2 * 1000,
						iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_cMapIndex, dX, dY,
						m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill], iSkillUsingTimeID, NULL);

					// 짹창쩌첬 쨩챌쩔챘횁횩 
					m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = TRUE;
					m_pClientList[iClientH]->m_iSkillUsingTimeID[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = iSkillUsingTimeID; //v1.12
				}
			}
		}
	}
}

BOOL CGame::bSetItemToBankItem(int iClientH, short sItemIndex)
{
	int i, iRet;
	DWORD* dwp;
	WORD* wp;
	char* cp;
	short* sp;
	char cData[100];
	class CItem* pItem;

	// 쩌횘횁철횉횕째챠 ��횜쨈횂 쩐횈��횑횇횤��쨩 쨘쨍째체횉횗쨈횢.
	if (m_pClientList[iClientH] == NULL) return FALSE;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;
	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return FALSE;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL) {
			// 쨘챰쩐챤��횜쨈횂 ��짠횆징쨍짝 횄짙쩐횘쨈횢.

			m_pClientList[iClientH]->m_pItemInBankList[i] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
			pItem = m_pClientList[iClientH]->m_pItemInBankList[i];
			// !!! 쩐횈��횑횇횤��횉 횈첨��횓횇횒쨍짝 ��횑쨉쩔횉횩��쨍쨈횕 짹창횁쨍��횉 횈첨��횓횇횒쨈횂 NULL째짧��쨍쨌횓 횉횘쨈챌. 
			m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;

			// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
			iCalcTotalWeight(iClientH);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMTOBANK;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			*cp = i; // ��짠횆징 ��첬��책 
			cp++;

			// 1째쨀.
			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0;
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			// v1.432
			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			// v1.42
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			//SNOOPY: Completion/Purity fix ?
			sp = (short*)cp;
			*cp = pItem->m_sItemSpecEffectValue2;
			cp += 2;

			// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 55 + 2);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢. v1.41 횁짝째횇횉횕횁철 쩐횎쨈횂쨈횢.
				break; // v1.41 FALSE쨍짝 쨔횦횊짱횉횕쨍챕 쩐횈��횑횇횤��횑 쨔횢쨈횣쩔징 쨘쨔쨩챌쨉횊쨈횢.
			}

			return TRUE;
		}

	// 쩐횈��횑횇횤��쨩 쨘쨍째체횉횘 쩔짤��짱째첩째짙��횑 쩐첩쨈횢.
	return FALSE;
}
BOOL CGame::bSetItemToBankItem(int iClientH, class CItem* pItem)
{
	int i, iRet;
	DWORD* dwp;
	WORD* wp;
	char* cp;
	short* sp;
	char cData[100];

	if (m_pClientList[iClientH] == NULL) return FALSE;
	if (pItem == NULL) return FALSE;
	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return FALSE;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL) {

			m_pClientList[iClientH]->m_pItemInBankList[i] = pItem;

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMTOBANK;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			*cp = i; // ��짠횆징 ��첬��책 
			cp++;

			// 1째쨀.
			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0;
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			// v1.432
			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			// v1.42
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			//SNOOPY: Completion/Purity fix ?
			sp = (short*)cp;
			*cp = pItem->m_sItemSpecEffectValue2;
			cp += 2;

			// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 55 + 2);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢. v1.41 횁짝째횇횉횕횁철 쩐횎쨈횂쨈횢.
				break; // v1.41 FALSE쨍짝 쨔횦횊짱횉횕쨍챕 쩐횈��횑횇횤��횑 쨔횢쨈횣쩔징 쨘쨔쨩챌쨉횊쨈횢.
			}

			return TRUE;
		}

	// 쩐횈��횑횇횤��쨩 쨘쨍째체횉횘 쩔짤��짱째첩째짙��횑 쩐첩쨈횢.
	return FALSE;
}

void CGame::ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char* pItemName)
{
	char cItemCategory, cItemName[21];
	short sRemainLife;
	int   iPrice;
	double d1, d2, d3;
	BOOL   bNeutral;
	DWORD  dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2, dwMul1, dwMul2;
	CItem* m_pGold;

	// 쨩챌쩔챘��횣��횉 쩐횈��횑횇횤 횈횊짹창 쩔채짹쨍.
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;
	if (iNum <= 0) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

	iCalcTotalWeight(iClientH);

	m_pGold = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(m_pGold, cItemName);

	// v1.42
	bNeutral = FALSE;
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = TRUE;
	// v2.13 쩌쨘횊횆쨈횕 쩌철횁짚 쨔째째횉��쨩 쩐챤쨉챨쩌짯쨀짧 횈횊째횚 쩌철횁짚횉횕쩔짤쩌짯 횈횊쨋짠쨈횂 NPC 짹쨍쨘횖��횑 횉횎쩔채쩐첩쨈횢. 
	// 쨈횥 횆짬횇횞째챠쨍짰쨍짝 짹창횁횠��쨍쨌횓 쩐횈��횑횇횤 째징째횦��쨩 째찼횁짚횉횗쨈횢.
	switch (cSellToWhom) {
	case 15: 		// 쨩처횁징 쩐횈횁횥쨍쨋 
	case 24:        // 쨈챘��책째짙 횁횜��횓 
		cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;
		// 12-22 쩌쨘횊횆쨈횕 쩌철횁짚 쩐챤쨉챨쩌짯쨉챌 횈횊쩌철 ��횜째횚 쩌철횁짚 
		// 쨩처횁징쩐횈��횑횇횤 
		if ((cItemCategory >= 11) && (cItemCategory <= 50)) {

			// ��청횉횛횉횕쨈횢. 쨔짬횁쨋째횉 쨔횦째짧 
			iPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) * iNum;
			sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;


			//v1.42 횁횩쨍쨀��횓 째챈쩔챙 쨔횦��횉 쨔횦째짧.
			if (bNeutral == TRUE) iPrice = iPrice / 2;
			if (iPrice <= 0)    iPrice = 1;
			if (iPrice > 1000000) iPrice = 1000000;

			if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH)) {
				// v2.12 횈횊 째챈쩔챙 쨔짬째횚째징 횄횎째첬쨉횉쩐챤쩌짯 횈횊 쩌철 쩐첩쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			}
			else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
		}
		// 쨈챘��책째짙 쩐횈��횑횇횤
		else if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
			// 쩔첩쨌징 쩐횈��횑횇횤��횉 쩌철쨍챠째첬 쨘챰짹쨀횉횠쩌짯 째짢째징 쨩처째짖��쨩 째챔쨩챗, 쩐횈��횑횇횤��횉 째징째횦��쨩 쨍횇짹채쨈횢.
			sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

			if (sRemainLife == 0) {
				// 째챠��책쨀짯 쩐횈��횑횇횤��쨘 횈횊 쩌철 쩐첩쨈횢
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			}
			else {
				d1 = (double)sRemainLife;
				if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
					d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
				else d2 = 1.0f;
				d3 = (d1 / d2) * 0.5f;
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 쩔첩쨌징 째징째횦 
				d3 = d3 * d2; // 횄횩쨩챗쨉횊 째징째횦 

				iPrice = (int)d3;
				iPrice = iPrice * iNum;

				dwAddPrice1 = 0;
				dwAddPrice2 = 0;
				// 쩐횈��횑횇횤 횈짱쩌쨘횆징쩔징 쨉청쨍짜 째징째횦 쨩처쩍횂 
				if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != NULL) {
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

					switch (dwSWEType) {
					case 6: dwMul1 = 2; break;  // 째징쨘짯쩔챤 
					case 8: dwMul1 = 2; break;  // 째짯횊짯쨉횊
					case 5: dwMul1 = 3; break;  // 쨔횓횄쨍��횉
					case 1: dwMul1 = 4; break;  // 횉횎쨩챙��횉 
					case 7: dwMul1 = 5; break;  // 쩔쨔쨍짰횉횗
					case 2: dwMul1 = 6; break;  // 횁횩쨉쨋��횉
					case 3: dwMul1 = 15; break; // 횁짚��횉��횉 
					case 9: dwMul1 = 20; break; // 째챠쨈챘쨔짰쨍챠 
					default: dwMul1 = 1; break;
					}

					d1 = (double)iPrice * dwMul1;
					switch (dwSWEValue) {
					case 1: d2 = 10.0f; break;
					case 2: d2 = 20.0f; break;
					case 3: d2 = 30.0f; break;
					case 4: d2 = 35.0f; break;
					case 5: d2 = 40.0f; break;
					case 6: d2 = 50.0f; break;
					case 7: d2 = 100.0f; break;
					case 8: d2 = 200.0f; break;
					case 9: d2 = 300.0f; break;
					case 10: d2 = 400.0f; break;
					case 11: d2 = 500.0f; break;
					case 12: d2 = 700.0f; break;
					case 13: d2 = 900.0f; break;
					default: d2 = 0.0f; break;
					}
					d3 = d1 * (d2 / 100.0f);

					dwAddPrice1 = (int)(d1 + d3);
				}

				// v1.42 횊챰짹횒 쩐횈��횑횇횤��횑쨋처쨍챕 Sub 횊쩔째첬쨍짝 쩌쨀횁짚횉횗쨈횢. 째첩째횦쨔짬짹창쨈횂 1째쨀쨍쨍 ��책횂첩쨉횊쨈횢째챠 횉횩��쨩쨋짠쨍쨍 ��짱횊쩔횉횚.
				if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != NULL) {
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

					switch (dwSWEType) {
					case 1:
					case 12: dwMul2 = 2; break;

					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7: dwMul2 = 4; break;

					case 8:
					case 9:
					case 10:
					case 11: dwMul2 = 6; break;
					}

					d1 = (double)iPrice * dwMul2;
					switch (dwSWEValue) {
					case 1: d2 = 10.0f; break;
					case 2: d2 = 20.0f; break;
					case 3: d2 = 30.0f; break;
					case 4: d2 = 35.0f; break;
					case 5: d2 = 40.0f; break;
					case 6: d2 = 50.0f; break;
					case 7: d2 = 100.0f; break;
					case 8: d2 = 200.0f; break;
					case 9: d2 = 300.0f; break;
					case 10: d2 = 400.0f; break;
					case 11: d2 = 500.0f; break;
					case 12: d2 = 700.0f; break;
					case 13: d2 = 900.0f; break;
					default: d2 = 0.0f; break;
					}
					d3 = d1 * (d2 / 100.0f);

					dwAddPrice2 = (int)(d1 + d3);
				}

				// v2.03 925 횈짱쩌철 쩐횈��횑횇횤 째징째횦 째징횁횩횆징쨍짝 77%쩌철횁횠��쨍쨌횓 쨈횢쩔챤 
				iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

				//v1.42 횁횩쨍쨀��횓 째챈쩔챙 쨔횦��횉 쨔횦째짧.
				if (bNeutral == TRUE) iPrice = iPrice / 2;
				if (iPrice <= 0)    iPrice = 1;
				if (iPrice > 1000000) iPrice = 1000000;

				if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH)) {
					// v2.12 횈횊 째챈쩔챙 쨔짬째횚째징 횄횎째첬쨉횉쩐챤쩌짯 횈횊 쩌철 쩐첩쨈횢.
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
				}
				else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
			}
		}
		else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 1, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
		break;


	default:
		break;
	}
	if (m_pGold != NULL) delete m_pGold;
}

void CGame::ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char* pString)
{
	class CItem* pItemGold;
	short sRemainLife;
	int   iPrice;
	double d1, d2, d3;
	char* cp, cItemName[21], cData[120], cItemCategory;
	DWORD* dwp, dwMul1, dwMul2, dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2;
	WORD* wp;
	int    iEraseReq, iRet;
	short* sp;
	BOOL   bNeutral;


	// 쩐횈��횑횇횤��쨩 횈횊째횣쨈횢쨈횂 째횒��횑 째찼횁짚쨉횉쩐첬쨈횢.
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;
	if (iNum <= 0) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

	// New 18/05/2004
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

	iCalcTotalWeight(iClientH);
	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// v1.42
	bNeutral = FALSE;
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = TRUE;

	iPrice = 0;
	// 쩐횈��횑횇횤��횉 횁쩐쨌첫쩔징 쨉청쨋처 째징째횦 째챔쨩챗.
	if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
		// 쨔짬짹창쨌첫쨈횢
		// 쩔첩쨌징 쩐횈��횑횇횤��횉 쩌철쨍챠째첬 쨘챰짹쨀횉횠쩌짯 째짢째징 쨩처째짖��쨩 째챔쨩챗, 쩐횈��횑횇횤��횉 째징째횦��쨩 쨍횇짹채쨈횢.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

		if (sRemainLife > 0) {
			// 쨍횁째징횁첩 쩐횈��횑횇횤��쨘 횈횊횁철 쨍첩횉횗쨈횢.	

			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 쩔첩쨌징 째징째횦 
			d3 = d3 * d2; // 횄횩쨩챗쨉횊 째징째횦 

			iPrice = (short)d3;
			iPrice = iPrice * iNum;

			dwAddPrice1 = 0;
			dwAddPrice2 = 0;
			// 쩐횈��횑횇횤 횈짱쩌쨘횆징쩔징 쨉청쨍짜 째징째횦 쨩처쩍횂 
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != NULL) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
				dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

				// 횊챰짹횒 쩐횈��횑횇횤 횊쩔째첬 횁쩐쨌첫: 
				// 0-None 1-횉횎쨩챙짹창쨈챘쨔횑횁철횄횩째징 2-횁횩쨉쨋횊쩔째첬 3-횁짚��횉��횉 4-��첬횁횜��횉 
				// 5-쨔횓횄쨍��횉 6-째징쨘짯쩔챤 7-쩔쨔쨍짰횉횗 8-째짯횊짯쨉횊 9-째챠쨈챘쨔짰쨍챠��횉
				switch (dwSWEType) {
				case 6: dwMul1 = 2; break;  // 째징쨘짯쩔챤 
				case 8: dwMul1 = 2; break;  // 째짯횊짯쨉횊
				case 5: dwMul1 = 3; break;  // 쨔횓횄쨍��횉
				case 1: dwMul1 = 4; break;  // 횉횎쨩챙��횉 
				case 7: dwMul1 = 5; break;  // 쩔쨔쨍짰횉횗
				case 2: dwMul1 = 6; break;  // 횁횩쨉쨋��횉
				case 3: dwMul1 = 15; break; // 횁짚��횉��횉 
				case 9: dwMul1 = 20; break; // 째챠쨈챘쨔짰쨍챠 
				default: dwMul1 = 1; break;
				}

				d1 = (double)iPrice * dwMul1;
				switch (dwSWEValue) {
				case 1: d2 = 10.0f; break;
				case 2: d2 = 20.0f; break;
				case 3: d2 = 30.0f; break;
				case 4: d2 = 35.0f; break;
				case 5: d2 = 40.0f; break;
				case 6: d2 = 50.0f; break;
				case 7: d2 = 100.0f; break;
				case 8: d2 = 200.0f; break;
				case 9: d2 = 300.0f; break;
				case 10: d2 = 400.0f; break;
				case 11: d2 = 500.0f; break;
				case 12: d2 = 700.0f; break;
				case 13: d2 = 900.0f; break;
				default: d2 = 0.0f; break;
				}
				d3 = d1 * (d2 / 100.0f);
				dwAddPrice1 = (int)(d1 + d3);
			}

			// v1.42 횊챰짹횒 쩐횈��횑횇횤��횑쨋처쨍챕 Sub 횊쩔째첬쨍짝 쩌쨀횁짚횉횗쨈횢. 째첩째횦쨔짬짹창쨈횂 1째쨀쨍쨍 ��책횂첩쨉횊쨈횢째챠 횉횩��쨩쨋짠쨍쨍 ��짱횊쩔횉횚.
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != NULL) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
				dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

				// 횊챰짹횒 쩐횈��횑횇횤 횊쩔째첬 횁쩐쨌첫: 
				//횄횩째징 쨉쨋쩌쨘��첬횉횞(1), 횄횩째징 쨍챠횁횩째짧(2), 횄횩째징 쨔챈쩐챤째짧(3), HP 횊쨍쨘쨔쨌짰 횄횩째징(4), SP 횊쨍쨘쨔쨌짰 횄횩째징(5)
				//MP 횊쨍쨘쨔쨌짰 횄횩째징(6), 횄횩째징 쨍쨋쨔첵��첬횉횞(7), 쨔째쨍짰 쨈챘쨔횑횁철 횊챠쩌철(8), 쨍쨋쨔첵 쨈챘쨔횑횁철 횊챠쩌철(9)
				//쩔짭횇쨍 쨈챘쨔횑횁철 횄횩째징(10), 쨈천 쨍쨔��쨘 째챈횉챔횆징(11), 쨈천쨍쨔��쨘 Gold(12)
				switch (dwSWEType) {
				case 1:
				case 12: dwMul2 = 2; break;

				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7: dwMul2 = 4; break;

				case 8:
				case 9:
				case 10:
				case 11: dwMul2 = 6; break;
				}

				d1 = (double)iPrice * dwMul2;
				switch (dwSWEValue) {
				case 1: d2 = 10.0f; break;
				case 2: d2 = 20.0f; break;
				case 3: d2 = 30.0f; break;
				case 4: d2 = 35.0f; break;
				case 5: d2 = 40.0f; break;
				case 6: d2 = 50.0f; break;
				case 7: d2 = 100.0f; break;
				case 8: d2 = 200.0f; break;
				case 9: d2 = 300.0f; break;
				case 10: d2 = 400.0f; break;
				case 11: d2 = 500.0f; break;
				case 12: d2 = 700.0f; break;
				case 13: d2 = 900.0f; break;
				default: d2 = 0.0f; break;
				}
				d3 = d1 * (d2 / 100.0f);
				dwAddPrice2 = (int)(d1 + d3);
			}

			iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

			//v1.42 횁횩쨍쨀��횓 째챈쩔챙 쨔횦��횉 쨔횦째짧.
			if (bNeutral == TRUE) iPrice = iPrice / 2;
			if (iPrice <= 0) iPrice = 1;
			if (iPrice > 1000000) iPrice = 1000000; // New 06/05/2004

			// 쩐횈��횑횇횤��쨩 횈횊쩐횘쨈횢쨈횂 쨍횧쩍횄횁철 ��체쩌횤 (쨈횢��횑쩐처쨌횓짹횞 쨔횣쩍쨘 쨘챰횊째쩌쨘횊짯쩔챘)
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, NULL, NULL, NULL);

			_bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

			// 횈횊 쩐횈��횑횇횤��쨩 쨩챔횁짝 
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
				(m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
				// 쩌철쨌짰 째쨀쨀채��횑 ��횜쨈횢쨍챕 째쨔쩌철쨍짝 횁횢��횓쨈횢.
				// v1.41 !!!
				SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
			}
			else ItemDepleteHandler(iClientH, cItemID, FALSE, FALSE);
		}
	}
	else if ((cItemCategory >= 11) && (cItemCategory <= 50)) {
		// 쩍횆횉째, ��창횊짯쨉챤��횉 쨔횦째짧쨔째째횉쨉챕 
		iPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		iPrice = iPrice * iNum;

		//v1.42 횁횩쨍쨀��횓 째챈쩔챙 쨔횦��횉 쨔횦째짧.
		if (bNeutral == TRUE) iPrice = iPrice / 2;
		if (iPrice <= 0) iPrice = 1;
		if (iPrice > 1000000) iPrice = 1000000; // New 06/05/2004

		// 쩐횈��횑횇횤��쨩 횈횊쩐횘쨈횢쨈횂 쨍횧쩍횄횁철 ��체쩌횤 (쨈횢��횑쩐처쨌횓짹횞 쨔횣쩍쨘 쨘챰횊째쩌쨘횊짯쩔챘)
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, NULL, NULL, NULL);

		_bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

		// 쩐횈��횑횇횤��횉 횁쩐쨌첫쩔징 쨉청쨋처 ��청��첵횉횗 횄쨀쨍짰쨍짝 횉횗쨈횢.
		if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
			// 쩌철쨌짰 째쨀쨀채��횑 ��횜쨈횢쨍챕 째쨔쩌철쨍짝 횁횢��횓쨈횢.
			// v1.41 !!!
			SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
		}
		else ItemDepleteHandler(iClientH, cItemID, FALSE, TRUE);
	}

	// Gold쨍짝 횁천째징쩍횄횇짼쨈횢. 쨍쨍쩐횪 횈횉 째징째횦��횑 0 횊짚��쨘 쨍쨋��횑쨀횎쩍쨘��횑쨍챕 짹횦��쨩 횁횜횁철 쩐횎쨈횂쨈횢.
	if (iPrice <= 0) return;

	pItemGold = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(pItemGold, cItemName);

	pItemGold->m_dwCount = iPrice;

	if (_bAddClientItemList(iClientH, pItemGold, &iEraseReq) == TRUE) {
		// 쩐횈��횑횇횤��쨩 횊쨔쨉챈횉횩쨈횢.

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		// 1째쨀 횊쨔쨉챈횉횩쨈횢. <- 쩔짤짹창쩌짯 1째쨀쨋천 횆짬쩔챤횈짰쨍짝 쨍쨩횉횕쨈횂 째횒��횑 쩐횈쨈횕쨈횢
		*cp = 1;
		cp++;

		memcpy(cp, pItemGold->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItemGold->m_dwCount;
		cp += 4;

		*cp = pItemGold->m_cItemType;
		cp++;

		*cp = pItemGold->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItemGold->m_sLevelLimit;
		cp += 2;

		*cp = pItemGold->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItemGold->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItemGold->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItemGold->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItemGold->m_sSpriteFrame;
		cp += 2;

		*cp = pItemGold->m_cItemColor;
		cp++;

		*cp = (char)pItemGold->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItemGold->m_dwAttribute;
		cp += 4;


		if (iEraseReq == 1)
			delete pItemGold;

		// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

		// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
		iCalcTotalWeight(iClientH);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}
	}
	else {
		// 횁횩쨌짰 횄횎째첬쨉챤��횉 쨔짰횁짝쨌횓 횄횩째징 쩍횉횈횖.
		// 쨔횧횁철 쨍첩횉횩��쨍쨔횉쨌횓 쨔횢쨈횣쩔징 쨋쨀쩐챤횁첩쨈횢. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY, pItemGold);

		// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItemGold->m_sSprite, pItemGold->m_sSpriteFrame, pItemGold->m_cItemColor); // v1.4 color

		// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
		iCalcTotalWeight(iClientH);

		// 쨈천��횑쨩처 째징횁첬쩌철 쩐첩쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}
	}
}

void CGame::ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char* pString)
{
	char cItemCategory;
	short sRemainLife, sPrice;
	double d1, d2, d3;

	// 쩐횈��횑횇횤��쨩 째챠횆징째횣쨈횢쨈횂 쩔채짹쨍.
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;

	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// 쩐횈��횑횇횤��횉 횁쩐쨌첫쩔징 쨉청쨋처 째징째횦 째챔쨩챗.
	if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
		// 쨔짬짹창쨌첫쨈횢

		// 쨍쨍쩐횪 쨔짬짹창쨍짝 쨈챘��책째짙 횁횜��횓��횑 쩐횈쨈횗 ��횑쩔징째횚 째챠횄횆쨈횧쨋처째챠 횉횗쨈횢쨍챕 횉횘 쩌철 쩐첩쨈횢. 
		if (cRepairWhom != 24) {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			return;
		}

		// 쩔첩쨌징 쩐횈��횑횇횤��횉 쩌철쨍챠째첬 쨘챰짹쨀횉횠쩌짯 째짢째징 쨩처째짖��쨩 째챔쨩챗, 쩐횈��횑횇횤��횉 쩌철쨍짰 쨘챰쩔챘��쨩 쨍횇짹채쨈횢.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 쩔횕��체횊첨 쨍횁째징횁첩 째횒��횑쨋처쨍챕 쩔첩쨌징째징째횦��횉 ��첵쨔횦��횑 쨉챌쨈횢. 
			sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 쩔첩쨌징 째징째횦 
			d3 = d3 * d2; // 횄횩쨩챗쨉횊 째징째횦 

			sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
	else if (((cItemCategory >= 43) && (cItemCategory <= 50)) || ((cItemCategory >= 11) && (cItemCategory <= 12))) {
		// 쨀짭쩍횄쨈챘, 째챤짹짧��횑 쨉챤째첬 째째��쨘 쨘챰쨔짬짹창쨌첫 쩌철쨍짰째징쨈횋 쩐횈��횑횇횤. 쩔횎, 쨘횓횄첨쨌첫

		// 쨍쨍쩐횪 쨩처횁징 횁횜��횓��횑 쩐횈쨈횗 ��횑쩔징째횚 째챠횄횆쨈횧쨋처째챠 횉횗쨈횢쨍챕 횉횘 쩌철 쩐첩쨈횢. 
		if (cRepairWhom != 15) {
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			return;
		}

		// 쩔첩쨌징 쩐횈��횑횇횤��횉 쩌철쨍챠째첬 쨘챰짹쨀횉횠쩌짯 째짢째징 쨩처째짖��쨩 째챔쨩챗, 쩐횈��횑횇횤��횉 쩌철쨍짰 쨘챰쩔챘��쨩 쨍횇짹채쨈횢.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 쩔횕��체횊첨 쨍횁째징횁첩 째횒��횑쨋처쨍챕 쩔첩쨌징째징째횦��횉 ��첵쨔횦��횑 쨉챌쨈횢. 
			sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 쩔첩쨌징 째징째횦 
			d3 = d3 * d2; // 횄횩쨩챗쨉횊 째징째횦 

			sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
	else {
		// 째챠횆짜쩌철 쩐첩쨈횂 쩐횈��횑횇횤��횑쨈횢.
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 1, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
}

void CGame::ReqRepairItemCofirmHandler(int iClientH, char cItemID, char* pString)
{
	short    sRemainLife, sPrice;
	char* cp, cItemCategory, cData[120];
	double   d1, d2, d3;
	DWORD* dwp, dwGoldCount;
	WORD* wp;
	int      iRet, iGoldWeight;

	// 쩐횈��횑횇횤��쨩 쩌철쨍짰횉횕째횣쨈횢쨈횂 째횒��횑 째찼횁짚쨉횉쩐첬쨈횢.
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;

	// New 18/05/2004
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// 쩐횈��횑횇횤��횉 횁쩐쨌첫쩔징 쨉청쨋처 째징째횦 째챔쨩챗.
	if (((cItemCategory >= 1) && (cItemCategory <= 10)) || ((cItemCategory >= 43) && (cItemCategory <= 50)) ||
		((cItemCategory >= 11) && (cItemCategory <= 12))) {
		// 쨔짬짹창쨌첫 횊짚��쨘 쨀짭쩍횄쨈챘, 째챤짹짧��횑쩔횒 째째��쨘 쩐횈��횑횇횤, 쩔횎, 쩍횇쨔횩 

		// 쩔첩쨌징 쩐횈��횑횇횤��횉 쩌철쨍챠째첬 쨘챰짹쨀횉횠쩌짯 째짢째징 쨩처째짖��쨩 째챔쨩챗, 쩐횈��횑횇횤��횉 쩌철쨍짰 쨘챰쩔챘��쨩 쨍횇짹채쨈횢.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 쩔횕��체횊첨 쨍횁째징횁첩 째횒��횑쨋처쨍챕 쩔첩쨌징째징째횦��횉 ��첵쨔횦��횑 쨉챌쨈횢. 
			sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)abs(sRemainLife);
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)abs(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan);
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 쩔첩쨌징 째징째횦 
			d3 = d3 * d2; // 횄횩쨩챗쨉횊 째징째횦 

			sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		// sPrice쨍쨍횇짯��횉 쨉쨌��횑 쨉횉쨍챕 째챠횆짜 쩌철 ��횜��쨍쨀짧 쨘횓횁쨌횉횕쨍챕 째챠횆짜 쩌철 쩐첩쨈횢. 
		dwGoldCount = dwGetItemCount(iClientH, "Gold");

		if (dwGoldCount < (DWORD)sPrice) {
			// 횉횄쨌쨔��횑쩐챤째징 째짰째챠��횜쨈횂 Gold째징 쩐횈��횑횇횤 쩌철쨍짰 쨘챰쩔챘쩔징 쨘챰횉횠 ��청쨈횢. 째챠횆짜 쩌철 쩐첩��쩍.
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_NOTENOUGHGOLD;
			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			*cp = cItemID;
			cp++;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
			return;
		}
		else {
			//쨉쨌��횑 횄챈쨘횖횉횕쨈횢. 째챠횆짜 쩌철 ��횜쨈횢. 

			// Centuu : Repair Fix (Recalculation of maxlifespan) - HB2
			//for (int i = 0; i < DEF_MAXITEMTYPES; i++) {
			//	if ((m_pItemConfigList[i] != NULL) &&
			//		(m_pItemConfigList[i]->m_sIDnum == m_pClientList[iClientH]->m_pItemList[cItemID]->m_sIDnum)) {
			//		m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
			//		if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != NULL) {
			//			dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
			//			dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;
			//			switch (dwSWEType) {
			//			case 8: // Strong
			//			case 9: // Ancient
			//				dV2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			//				dV3 = (double)(dwSWEValue * 7);
			//				dV1 = (dV3 / 100.0f) * dV2;
			//				m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan += (int)dV1;
			//				break;
			//			}
			//		}
			//	}
			//}

			// 쩐횈��횑횇횤��횉 쩌철쨍챠��쨩 쨈횄쨍짰째챠 횇챘쨘쨍횉횗쨈횢. !BUG POINT ��짠횆징째징 횁횩쩔채횉횕쨈횢. 쨍횛��첬 쩌철쨍챠��쨩 쨈횄쨍짰째챠 쨉쨌��횉 횆짬쩔챤횈짰쨍짝 쨀쨌횄찼쨈횢.
			m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMREPAIRED, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan, NULL, NULL);

			iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - sPrice);

			// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
			iCalcTotalWeight(iClientH);


		}
	}
}

int CGame::iCalcTotalWeight(int iClientH)
{
	int i, iWeight;
	short sItemIndex;

	if (m_pClientList[iClientH] == NULL) return 0;

	m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
	// 횂첩쩔챘횉횕횁철 쩐횎쩐횈쨉쨉 횊쩔째첬째징 ��횜쨈횂 쩐횈��횑횇횤 횊쩔째첬. 쩌횘횁철횉횕째챠 ��횜쩐챤쨉쨉 횊쩔째첬째징 ��횜짹창 쨋짠쨔짰쩔징 쩔짤짹창쩌짯 째횏쨩챌.
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) {
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0) {
					// 쩌철쨍챠��횑 ��횜쩐챤쩐횩 횊쩔째첬째징 ��횜쨈횢.
					m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
				}
				break;
			}
		}

	iWeight = 0;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {

			iWeight += iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], m_pClientList[iClientH]->m_pItemList[i]->m_dwCount);
		}

	m_pClientList[iClientH]->m_iCurWeightLoad = iWeight;

	return iWeight;
}

// 05/29/2004 - Hypnotoad - Purchase Dicount updated to take charisma into consideration
void CGame::RequestPurchaseItemHandler(int iClientH, char* pItemName, int iNum)
{
	class CItem* pItem;
	char* cp, cItemName[21], cData[100];
	short* sp;
	DWORD* dwp, dwGoldCount, dwItemCount;
	WORD* wp, wTempPrice;
	int   i, iRet, iEraseReq, iGoldWeight;
	int   iCost, iCost2, iDiscountRatio, iDiscountCost;
	double dTmp1, dTmp2, dTmp3;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	// 만약 아이템을 구입하고자 하는 곳이 자신의 마을이 아니라면 구입할 수 없다. 


	// 아이템을 구입한다. 
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cItemName, sizeof(cItemName));


	// 임시코드다. 
	if (memcmp(pItemName, "10Arrows", 8) == 0) {
		strcpy(cItemName, "Arrow");
		dwItemCount = 10;
	}
	else if (memcmp(pItemName, "100Arrows", 9) == 0) {
		strcpy(cItemName, "Arrow");
		dwItemCount = 100;
	}
	else {
		memcpy(cItemName, pItemName, 20);
		dwItemCount = 1;
	}

	for (i = 1; i <= iNum; i++) {

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == FALSE) {
			// 구입하고자 하는 아이템이 아이템 리스트상에 없다. 구입이 불가능하다.
			delete pItem;
		}
		else {

			if (pItem->m_bIsForSale == FALSE) {
				// 판매되는 아이템이 아니다. 살 수 없다.
				delete pItem;
				return;
			}

			pItem->m_dwCount = dwItemCount;


			//Heldenian Price Fix Thing
			if (m_pClientList[iClientH]->m_cSide == m_sLastHeldenianWinner) 
			{
				iCost = (int)((float)(pItem->m_wPrice) * 0.9f + 0.5f);
				iCost = iCost * pItem->m_dwCount;
				iCost2 = pItem->m_wPrice * pItem->m_dwCount;
			}
			else 
			{
				iCost2 = iCost = (pItem->m_wPrice * pItem->m_dwCount); //LifeX Fix gold price 11/2
			}

			// 플레이어가 소지한 Gold가 아이템을 사기에 충분한지 검사한다.
			dwGoldCount = dwGetItemCount(iClientH, "Gold");

			// Charisma에 따른 할인률을 계산한다. 
			// v2.14 카리스마가 10인경우 아이템을 할인 하지 않음 
			iDiscountRatio = (int)((m_pClientList[iClientH]->m_iCharisma - 10) / 4);

			dTmp1 = (double)(iDiscountRatio);
			dTmp2 = dTmp1 / 100.0f;
			dTmp1 = (double)iCost;
			dTmp3 = dTmp1 * dTmp2;
			iDiscountCost = (int)dTmp3;

			//Fix For Heldenian Price =x	
			if ((iCost - iDiscountCost) <= (int)(iCost2 / 2)) 
			{
				iDiscountCost = (int)(iCost - (iCost2 / 2) + 1);
			}


			if (dwGoldCount < (DWORD)(iCost - iDiscountCost)) {
				// 플레이어가 갖고있는 Gold가 아이템 가격에 비해 적다. 살수 없음.
				delete pItem;

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_NOTENOUGHGOLD;
				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				*cp = -1; // -1이면 의미없다.
				cp++;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					// 메시지를 보낼때 에러가 발생했다면 제거한다.
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
				return;
			}

			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
				// 에러 방지용 코드
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				// 아이템 샀다는 메시지를 전송한다.
				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_ITEMPURCHASED;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				// 1개 획득했다.
				*cp = 1;
				cp++;

				memcpy(cp, pItem->m_cName, 20);
				cp += 20;

				dwp = (DWORD*)cp;
				*dwp = pItem->m_dwCount;
				cp += 4;

				*cp = pItem->m_cItemType;
				cp++;

				*cp = pItem->m_cEquipPos;
				cp++;

				*cp = (char)0; // 얻은 아이템이므로 장착되지 않았다.
				cp++;

				sp = (short*)cp;
				*sp = pItem->m_sLevelLimit;
				cp += 2;

				*cp = pItem->m_cGenderLimit;
				cp++;

				wp = (WORD*)cp;
				*wp = pItem->m_wCurLifeSpan;
				cp += 2;

				wp = (WORD*)cp;
				*wp = pItem->m_wWeight;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSprite;
				cp += 2;

				sp = (short*)cp;
				*sp = pItem->m_sSpriteFrame;
				cp += 2;

				*cp = pItem->m_cItemColor;
				cp++;

				wp = (WORD*)cp;
				*wp = (iCost - iDiscountCost);
				wTempPrice = (iCost - iDiscountCost);
				cp += 2;

				if (iEraseReq == 1) delete pItem;

				// 아이템 정보 전송 
				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 48);

				// Gold의 수량을 감소시킨다. 반드시 여기서 세팅해야 순서가 바뀌지 않는다.
				iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - wTempPrice);
				// 소지품 총 중량 재 계산 
				iCalcTotalWeight(iClientH);



				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					// 메시지를 보낼때 에러가 발생했다면 제거한다.
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
			}
			else
			{
				// 공간이 부족해 아이템을 얻을 수 없다.
				delete pItem;

				// 소지품 총 중량 재 계산 
				iCalcTotalWeight(iClientH);

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					// 메시지를 보낼때 에러가 발생했다면 제거한다.
					DeleteClient(iClientH, TRUE, TRUE);
					break;
				}
			}
		}
	}
}

void CGame::GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char* pItemName)
{
	int iRet, iEraseReq;
	short* sp, sOwnerH;
	char* cp, cOwnerType, cData[100], cCharName[21];
	DWORD* dwp;
	WORD* wp;
	class CItem* pItem;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
	if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (iAmount <= 0) return;

	// 쩐횈��횑횇횤 ��횑쨍짠��횑 ��횕횆징횉횕횁철 쩐횎쩐횈쨉쨉 쨔짬쩍횄쨉횊쨈횢.
	if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) {
		PutLogList("(X) GiveItemHandler - Not matching Item name");
		return;
	}

	ZeroMemory(cCharName, sizeof(cCharName));

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount > (DWORD)iAmount)) {
		// 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��횑쩐첬째챠 쩌철쨌짰쨍쨍횇짯 째짢쩌횘쩍횄횇째째챠 쨀짼��쨘 째횚 ��횜쨈횢쨍챕 

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == FALSE) {
			// 쨘횖횉횘횉횕째챠��횣 횉횕쨈횂 쩐횈��횑횇횤��횑 쨍짰쩍쨘횈짰쩔징 쩐첩쨈횂 째횇쨈횢. ��횑쨌짹 ��횕��쨘 ��횕쩐챤쨀짱 쩌철째징 쩐첩횁철쨍쨍 
			delete pItem;
			return;
		}
		else {
			pItem->m_dwCount = iAmount;
		}

		// 쩌철쨌짰 째짢쩌횘: 0쨘쨍쨈횢 횇짤쨈횢.
		m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

		// 쨘짱째챈쨉횊 쩌철쨌짰��쨩 쩌쨀횁짚횉횕째챠 쩐횏쨍째쨈횢.
		// v1.41 !!! 쩐횈��횑횇횤 ��횑쨍짠쩔징쩌짯 ��횓쨉짝쩍쨘쨌횓 쨘짱째챈쨉횉쩐첬쨈횢. 
		SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

		//��횑횁짝 dX, dY쩔징 ��횜쨈횂 쩔��쨘챗횁짠횈짰쩔징째횚 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��쨩 째횉쨀횞횁횠쨈횢. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

		// v1.4 횁횜째챠��횣 횉횗 째쨈횄쩌쩔횒 쨍횂쨈횂횁철 횈횉쨈횥횉횗쨈횢.
		if (wObjectID != NULL) {
			if (wObjectID < 10000) {
				// 횉횄쨌쨔��횑쩐챤 
				if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS)) {
					if (m_pClientList[wObjectID] != NULL) {
						if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
					}
				}
			}
			else {
				// NPC
				if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS)) {
					if (m_pNpcList[wObjectID - 10000] != NULL) {
						if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = NULL;
					}
				}
			}
		}

		// Snoopy: Bock attemps to give item to a far character.
		if (((abs(m_pClientList[iClientH]->m_sX) - dX) > 5)
			|| ((abs(m_pClientList[iClientH]->m_sY) - dY) > 5))
		{
			sOwnerH = NULL;
		}

		if (sOwnerH == NULL) {
			// 횁횜째챠��횣 횉횕쨈횂 ��짠횆징쩔징 쩐횈쨔짬쨉쨉 쩐첩쨈횢.
			// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

			// v1.411  
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

			// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color
		}
		else {
			// 쩐횈��횑횇횤��쨩 횁횠쨈횢.
			if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
				// 횉횄쨌쨔��횑쩐챤쩔징째횚 횁횜쩐첬쨈횢.
				memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);

				if (sOwnerH == iClientH) {
					// 쨍쨍쩐횪 ��횣짹창 ��횣쩍횇쩔징째횚 횁횜쨈횂 째횇쨋처쨍챕 쨔짬쩍횄횉횗쨈횢. 횉횠횇쨌��횉 쩌횘횁철째징 ��횜쨈횢.
					delete pItem;
					return;
				}

				if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == TRUE) {
					// 쩐횈��횑횇횤��쨩 횊쨔쨉챈횉횩쨈횢.
					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_ITEMOBTAINED;

					cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

					// 1째쨀 횊쨔쨉챈횉횩쨈횢. Amount째징 쩐횈쨈횕쨈횢!
					*cp = 1;
					cp++;

					memcpy(cp, pItem->m_cName, 20);
					cp += 20;

					dwp = (DWORD*)cp;
					*dwp = pItem->m_dwCount;	// 쩌철쨌짰��쨩 ��횚쨌횂 
					cp += 4;

					*cp = pItem->m_cItemType;
					cp++;

					*cp = pItem->m_cEquipPos;
					cp++;

					*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
					cp++;

					sp = (short*)cp;
					*sp = pItem->m_sLevelLimit;
					cp += 2;

					*cp = pItem->m_cGenderLimit;
					cp++;

					wp = (WORD*)cp;
					*wp = pItem->m_wCurLifeSpan;
					cp += 2;

					wp = (WORD*)cp;
					*wp = pItem->m_wWeight;
					cp += 2;

					sp = (short*)cp;
					*sp = pItem->m_sSprite;
					cp += 2;

					sp = (short*)cp;
					*sp = pItem->m_sSpriteFrame;
					cp += 2;

					*cp = pItem->m_cItemColor;
					cp++;

					*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
					cp++;

					dwp = (DWORD*)cp;
					*dwp = pItem->m_dwAttribute;
					cp += 4;


					if (iEraseReq == 1) delete pItem;

					// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 53);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(sOwnerH, TRUE, TRUE);
						break;
					}

					// v1.4 쩌철쨌짰쨈횥��짠��횉 쩐횈��횑횇횤��쨩 ��체쨈횧횉횗 째횒��쨩 쩐횏쨌횁횁횠쨈횢. 
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED, sItemIndex, iAmount, NULL, cCharName);
				}
				else {
					// 쩐횈��횑횇횤��쨩 ��체쨈횧쨔횧��쨘 횆쨀쨍짱횇횒째징 쨈천��횑쨩처 쩐횈��횑횇횤��쨩 쨘쨍째체횉횘 쩌철 쩐첩쨈횂 쨩처횇횂��횑쨈횢.
					// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						pItem);

					// v1.411  
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

					// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

// 쨈천��횑쨩처 째징횁첬쩌철 쩐첩쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 6);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(sOwnerH, TRUE, TRUE);
						break;
					}

					// v1.4 쩌철쨌짰쨈횥��짠��횉 쩐횈��횑횇횤��쨩 ��체쨈횧쩔징 쩍횉횈횖횉횩��쩍��쨩 쩐횏쨍째쨈횢.
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTGIVEITEM, sItemIndex, iAmount, NULL, cCharName);
				}

			}
			else {
				// NPC쩔징째횚 쩐횈��횑횇횤��쨩 횁횜쩐첬쨈횢.
				memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

				if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0) {
					// NPC째징 횄짖째챠 횁횜��횓��횑쩐첬쨈횢쨍챕 쨔째째횉��쨩 쨘쨍째체횉횕째횣쨈횢쨈횂 ��횉쨔횑��횑쨈횢. 
					if (bSetItemToBankItem(iClientH, pItem) == FALSE) {
						// 쨔째째횉��쨩 쨍횄짹창쨈횂쨉짜 쩍횉횈횖횉횕쩔쨈쨈횢.	
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, NULL, NULL, NULL, NULL);

						// 쩍횉횈횖횉횩��쨍쨔횉쨌횓 쨔횢쨈횣쩔징 쨋쨀짹쨘쨈횢.
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

						// v1.411  
						_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

						// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color
					}
				}
				else {
					// ��횕쨔횦 NPC쩔징째횚 쩐횈��횑횇횤��쨩 횁횜쨍챕 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨌횁쩐횩 횉횗쨈횢. 
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

					// v1.411  
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

					// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color
				}
			}
		}
	}
	else {
		// 쩐횈��횑횇횤 ��체쨘횓쨍짝 횁횜쩐첬쨈횢.

		// ��책횂첩횁횩��횓 쩐횈��횑횇횤��쨩 횁횜쩐첬쨈횢쨍챕 쩐횈��횑횇횤 ��책횂첩횊쩔째첬쨍짝 횉횠횁짝횉횠쩐횩 횉횕쨔횉쨌횓.
		ReleaseItemHandler(iClientH, sItemIndex, TRUE);

		// 횄쨀쨍짰쨉쨉횁횩 쩔징쨌짱째징 쨔횩쨩첵횉횘쨋짠쨍짝 쨈챘쨘챰횉횠쩌짯 -1쨌횓 횉횘쨈챌횉횠 쨀천쨈횂쨈횢.
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
			m_pClientList[iClientH]->m_cArrowIndex = -1;

		//��횑횁짝 dX, dY쩔징 ��횜쨈횂 쩔��쨘챗횁짠횈짰쩔징째횚 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��쨩 째횉쨀횞횁횠쨈횢. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY); // 쩐횈��횑횇횤��쨩 dX, dY쩔징 ��횜쨈횂 횆쨀쨍짱횇횒쩔징째횚 횁횠쨈횢. 쨍쨍쩐횪 쨔횧��쨩 횆쨀쨍짱횇횒째징 쩐횈��횑횇횤��쨩 쨔횧횁철 쨍첩횉횘 쨩처횊짼��횑쨋처쨍챕 쨋짜쩔징 쨋쨀쩐챤횁첩쨈횢.  

		// v1.4 횁횜째챠��횣 횉횗 째쨈횄쩌쩔횒 쨍횂쨈횂횁철 횈횉쨈횥횉횗쨈횢.
		if (wObjectID != NULL) {
			if (wObjectID < 10000) {
				// 횉횄쨌쨔��횑쩐챤 
				if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS)) {
					if (m_pClientList[wObjectID] != NULL) {
						if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
					}
				}
			}
			else {
				// NPC
				if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS)) {
					if (m_pNpcList[wObjectID - 10000] != NULL) {
						if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = NULL;
					}
				}
			}
		}

		// Snoopy: Bock attemps to give item to a far character.
		if (((abs(m_pClientList[iClientH]->m_sX) - dX) > 5)
			|| ((abs(m_pClientList[iClientH]->m_sY) - dY) > 5))
		{
			sOwnerH = NULL;
		}

		if (sOwnerH == NULL) {
			// 쩐횈��횑횇횤��쨩 횁횜째챠��횣 횉횕쨈횂 ��책쩌횘쩔징 횆쨀쨍짱횇횒째징 쩐첩쨈횢. 
			// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
				m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			// v1.411  
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

			// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); // v1.4 color

// ��횑횁짝 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁쨀��쨍쨔횉쨌횓 쨍짰쩍쨘횈짰쩔징쩌짯 쨩챔횁짝횉횘째횒��쨩 횇챘쨘쨍횉횗쨈횢.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, NULL);
		}
		else {
			// 쩐횈��횑횇횤��쨩 쨈횢쨍짜 횆쨀쨍짱횇횒쩔징째횚 횁횠쨈횢. @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

			if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
				// 캐릭터에게 아이템을 주었다.	
				memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);
				pItem = m_pClientList[iClientH]->m_pItemList[sItemIndex];

				// v2.03 크루세이드 모드인경우도 길드 가입은 가능하다.

				// v2.17 2002-7-31 아이템을 아이템 고유번호로 생성할 수 있게 한다.
				if (pItem->m_sIDnum == 88) {

					// 플레이어 iClientH 가 sOwnerH에게 길드가입 신청서를 주었다. 만약 
					// sOwnerH가 길드마스터라면 아이템을 받는 것이 아니라 확인을 해 주어야 한다.
								// v2.17 2002-7-31 아이템을 아이템 고유번호로 생성할 수 있게 한다.
					if ((m_pClientList[iClientH]->m_iGuildRank == -1) &&
						(memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) &&
						(memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[sOwnerH]->m_cLocation, 10) == 0) &&
						(m_pClientList[sOwnerH]->m_iGuildRank == 0)) {
						// 길드 마스터이다.	길드 마스터에게는 가입확인 요청 메시지를 전송한다.
						SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION, NULL, NULL, NULL, NULL);
						// 이제 아이템을 준 본인에게 아이템을 주었으므로 리스트에서 삭제할것을 통보한다.
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, NULL, cCharName);

						_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

						goto REMOVE_ITEM_PROCEDURE;
					}
				}

				// v2.17 2002-7-31 아이템을 아이템 고유번호로 생성할 수 있게 한다.
				if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89)) {

					// 플레이어 iClientH 가 sOwnerH에게 길드탈퇴 신청서를 주었다. 만약 
					// sOwnerH가 길드마스터이고 iClientH와 길드 이름이 같고 iClientH가 길드원이라면 
					// 아이템을 받는 것이 아니라 확인을 해 주어야 한다.
					if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
						(m_pClientList[iClientH]->m_iGuildRank != -1) &&
						(m_pClientList[sOwnerH]->m_iGuildRank == 0)) {
						// 길드 마스터이다.	길드 마스터에게는 탈퇴확인 요청 메시지를 전송한다.
						SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION, NULL, NULL, NULL, NULL);
						// 이제 아이템을 준 본인에게 아이템을 주었으므로 리스트에서 삭제할것을 통보한다.
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, NULL, cCharName);

						_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

						goto REMOVE_ITEM_PROCEDURE;
					}
				}

				// ��횕쨔횦��청��횓 째챈쩔챙 쩐횈��횑횇횤��쨩 짹횞��첬 쨔횧��쨩 쨩횙...
				if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == TRUE) {

					// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 ��체쨈횧횉횗 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
					_bItemLog(DEF_ITEMLOG_GIVE, iClientH, sOwnerH, pItem);

					// 쩐횈��횑횇횤��쨩 횊쨔쨉챈횉횩쨈횢.
					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_ITEMOBTAINED;

					cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

					// 1째쨀 횊쨔쨉챈횉횩쨈횢.
					*cp = 1;
					cp++;

					memcpy(cp, pItem->m_cName, 20);
					cp += 20;

					dwp = (DWORD*)cp;
					*dwp = pItem->m_dwCount;
					cp += 4;

					*cp = pItem->m_cItemType;
					cp++;

					*cp = pItem->m_cEquipPos;
					cp++;

					*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
					cp++;

					sp = (short*)cp;
					*sp = pItem->m_sLevelLimit;
					cp += 2;

					*cp = pItem->m_cGenderLimit;
					cp++;

					wp = (WORD*)cp;
					*wp = pItem->m_wCurLifeSpan;
					cp += 2;

					wp = (WORD*)cp;
					*wp = pItem->m_wWeight;
					cp += 2;

					sp = (short*)cp;
					*sp = pItem->m_sSprite;
					cp += 2;

					sp = (short*)cp;
					*sp = pItem->m_sSpriteFrame;
					cp += 2;

					*cp = pItem->m_cItemColor;
					cp++;

					*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
					cp++;

					dwp = (DWORD*)cp;
					*dwp = pItem->m_dwAttribute;
					cp += 4;


					if (iEraseReq == 1) delete pItem;

					// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 53);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(sOwnerH, TRUE, TRUE);
						break;
					}
				}
				else {
					// 쩐횈��횑횇횤��쨩 ��체쨈횧쨔횧��쨘 횆쨀쨍짱횇횒째징 쨈천��횑쨩처 쩐횈��횑횇횤��쨩 쨘쨍째체횉횘 쩌철 쩐첩쨈횂 쨩처횇횂��횑쨈횢.
					// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]);
					// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 쨋쨀쩐챤쨋횩쨍째 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); // v1.4 color

// 쨈천��횑쨩처 째징횁첬쩌철 쩐첩쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 6);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(sOwnerH, TRUE, TRUE);
						break;
					}

					// v1.4 쩐횈��횑횇횤 ��체쨈횧��횑 쩍횉횈횖횉횩��쩍��쨩 쩐횏쨍짰쨈횂 쨔챈쨔첵 
					ZeroMemory(cCharName, sizeof(cCharName));
				}
			}
			else {
				// NPC쩔징째횚 쩐횈��횑횇횤��쨩 횁횜쩐첬쨈횢.
				memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

				if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0) {
					// NPC째징 횄짖째챠 횁횜��횓��횑쩐첬쨈횢쨍챕 쨔째째횉��쨩 쨘쨍째체횉횕째횣쨈횢쨈횂 ��횉쨔횑��횑쨈횢. 
					if (bSetItemToBankItem(iClientH, sItemIndex) == FALSE) {
						// 쩐횈��횑횇횤��쨩 쨘쨍째체횉횕쨈횂쨉짜 쩍횉횈횖횉횕쩔짤 쨔횢쨈횣쩔징 쨋쨀쩐챤횁쨀쨈횢.
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, NULL, NULL, NULL, NULL);

						// 쩍횉횈횖횉횩��쨍쨔횉쨌횓 쨔횢쨈횣쩔징 쨋쨀짹쨘쨈횢.
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
							m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 쨋쨀쩐챤쨋횩쨍째 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
						_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); // v1.4 color
					}
				}
				else if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Kennedy", 7) == 0) {
					// 짹챈쨉책 쨩챌쨔짬��책쩔징째횚 짹챈쨉책 횇쨩횇챨 쩍횇횄쨩쩌짯쨍짝 횁횜쩐첬쨈횢쨍챕 짹챈쨉책쨍짝 횇쨩횇챨횉횗쨈횢. 
					if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89)) {

						if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (m_pClientList[iClientH]->m_iGuildRank != -1)) {
							// 횇쨩횇챨 쩍횇횄쨩��횣쩔징째횚 횇쨩횇챨 쩌쨘째첩횉횩��쩍��쨩 쩐횏쨍짰쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쨩횁횠쨈횢.
							SendNotifyMsg(iClientH, iClientH, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, NULL, NULL, NULL, NULL);

							// 짹챈쨉책 횇쨩횇챨. 
							ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
							memcpy(m_pClientList[iClientH]->m_cGuildName, "NONE", 4);
							m_pClientList[iClientH]->m_iGuildRank = -1;
							m_pClientList[iClientH]->m_iGuildGUID = -1;

							// 횈짱쩌쨘��횑 쨔횢짼챤쨔횉쨌횓 쩔횥쩐챌��쨩 쨩천쨌횓 쨘쨍쨀쩍쨈횢. 
							SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);


						}

						// ��횑 쩐횈��횑횇횤��쨘 쨩챔횁짝쩍횄횆횗쩐횩 횉횗쨈횢. 
						delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
					}
					else {
						// 짹챈쨉책 횇쨩횇챨 쩍횇횄쨩쩌짯째징 쩐횈쨈횕쨋처쨍챕 쨔횢쨈횣쩔징 쨋쨀짹쨘쨈횢. 횇짤쨌챌쩌쩌��횑쨉책 쨍챨쨉책��횕쨋짠쨉쨉 짹챈쨉책 째체쨌횄 쩔짭쨩챗 쨘횘째징 
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
							m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 쨋쨀쩐챤쨋횩쨍째 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
						_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); // v1.4 color

	// v1.4 쩐횈��횑횇횤 ��체쨈횧��횑 쩍횉횈횖횉횩��쩍��쨩 쩐횏쨍짰쨈횂 쨔챈쨔첵 
						ZeroMemory(cCharName, sizeof(cCharName));

					}
				}
				else {
					// ��횕쨔횦 NPC쩔징째횚 쩐횈��횑횇횤��쨩 횁횜쨍챕 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨌횁쩐횩 횉횗쨈횢. 

					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 쨋쨀쩐챤쨋횩쨍째 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); // v1.4 color

// v1.4 쩐횈��횑횇횤 ��체쨈횧��횑 쩍횉횈횖횉횩��쩍��쨩 쩐횏쨍짰쨈횂 쨔챈쨔첵 
					ZeroMemory(cCharName, sizeof(cCharName));
				}
			}

			// ��횑횁짝 쩐횈��횑횇횤��쨩 횁횠 쨘쨩��횓쩔징째횚 쩐횈��횑횇횤��쨩 횁횜쩐첬��쨍쨔횉쨌횓 쨍짰쩍쨘횈짰쩔징쩌짯 쨩챔횁짝횉횘째횒��쨩 횇챘쨘쨍횉횗쨈횢.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, cCharName);
		}

	REMOVE_ITEM_PROCEDURE:;

		// 쨀횞횈짰쩔철횇짤 쩔��쨌첫쨌횓 횄쨀쨍짰쨉쨉횁횩 횇짭쨋처��횑쩐챨횈짰째징 횁짝째횇쨉횉쩐첬쨈횢쨍챕 쨈천��횑쨩처 횁첩횉횪횉횘 쩌철 쩐첩쨈횢. 
		if (m_pClientList[iClientH] == NULL) return;

		// 쩐횈��횑횇횤��쨩 횁횜째횇쨀짧 쨔철쨌횊��쨍쨔횉쨌횓 횁철쩔챤쨈횢. delete횉횠쩌짯쨈횂 쩐횊쨉횊쨈횢! 
		m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
		m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

		// 횊짯쨩챙 ��횓쨉짝쩍쨘쨍짝 ��챌 횉횘쨈챌
		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}

	// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
	iCalcTotalWeight(iClientH);
}

DWORD CGame::dwGetItemCount(int iClientH, char* pName)
{
	int i;
	char cTmpName[21];

	if (m_pClientList[iClientH] == NULL) return NULL;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pName);

	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0)) {

			return m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
		}

	return 0;
}

int CGame::SetItemCount(int iClientH, char* pItemName, DWORD dwCount)
{
	int i;
	char cTmpName[21];
	WORD wWeight;

	if (m_pClientList[iClientH] == NULL) return -1;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pItemName);

	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0)) {

			wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], 1);

			// 횆짬쩔챤횈짰째징 0��횑쨍챕 쨍챨쨉횓 쩌횘쨍챨쨉횊 째횒��횑쨔횉쨌횓 쨍짰쩍쨘횈짰쩔징쩌짯 쨩챔횁짝횉횗쨈횢.
			if (dwCount == 0) {
				ItemDepleteHandler(iClientH, i, FALSE, TRUE);
			}
			else {
				// 쩐횈��횑횇횤��횉 쩌철쨌짰��횑 쨘짱째챈쨉횉쩐첬��쩍��쨩 쩐횏쨍째쨈횢. 
				m_pClientList[iClientH]->m_pItemList[i]->m_dwCount = dwCount;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, i, dwCount, (char)TRUE, NULL);
			}

			return wWeight;
		}

	return -1;
}


int CGame::SetItemCount(int iClientH, int iItemIndex, DWORD dwCount)
{
	WORD wWeight;

	if (m_pClientList[iClientH] == NULL) return -1;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return -1;

	wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1);

	// 횆짬쩔챤횈짰째징 0��횑쨍챕 쨍챨쨉횓 쩌횘쨍챨쨉횊 째횒��횑쨔횉쨌횓 쨍짰쩍쨘횈짰쩔징쩌짯 쨩챔횁짝횉횗쨈횢.
	if (dwCount == 0) {
		ItemDepleteHandler(iClientH, iItemIndex, FALSE, TRUE);
	}
	else {
		// 쩐횈��횑횇횤��횉 쩌철쨌짰��횑 쨘짱째챈쨉횉쩐첬��쩍��쨩 쩐횏쨍째쨈횢. 
		m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount = dwCount;
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, iItemIndex, dwCount, (char)TRUE, NULL);
	}

	return wWeight;
}

// New 16/05/2004
void CGame::ReqCreateSlateHandler(int iClientH, char* pData)
{
	int i, iRet;
	short* sp;
	char cItemID[4], ctr[4];
	char* cp, cSlateColour, cData[120];
	BOOL bIsSlatePresent = FALSE;
	CItem* pItem;
	int iSlateType, iEraseReq;
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;

	for (i = 0; i < 4; i++) {
		cItemID[i] = 0;
		ctr[i] = 0;
	}

	cp = (char*)pData;
	cp += 11;

	// 14% chance of creating slates
	if (iDice(1, 100) < m_sSlateSuccessRate) bIsSlatePresent = TRUE;

	try {
		// make sure slates really exist
		for (i = 0; i < 4; i++) {
			cItemID[i] = *cp;
			cp++;

			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] == NULL || cItemID[i] > DEF_MAXITEMS) {
				bIsSlatePresent = FALSE;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
				return;
			}

			//No duping
			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 868) // LU
				ctr[0] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 869) // LD
				ctr[1] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 870) // RU
				ctr[2] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 871) // RD
				ctr[3] = 1;
		}
	}
	catch (...) {
		//Crash Hacker Caught
		bIsSlatePresent = FALSE;
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
		wsprintf(G_cTxt, "TSearch Slate Hack: (%s) Player: (%s) - creating slates without correct item!", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
		PutHackLogFileList(G_cTxt);
		DeleteClient(iClientH, TRUE, TRUE);
		return;
	}

	// Are all 4 slates present ??
	if (ctr[0] != 1 || ctr[1] != 1 || ctr[2] != 1 || ctr[3] != 1) {
		bIsSlatePresent = FALSE;
		return;
	}

	if (m_pClientList[iClientH]->m_iAdminUserLevel > 3) bIsSlatePresent = TRUE;

	// if we failed, kill everything
	if (!bIsSlatePresent) {
		for (i = 0; i < 4; i++) {
			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != NULL) {
				ItemDepleteHandler(iClientH, cItemID[i], FALSE, FALSE);
			}
		}
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
		return;
	}

	// make the slates
	for (i = 0; i < 4; i++) {
		if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != NULL) {
			ItemDepleteHandler(iClientH, cItemID[i], FALSE, FALSE);
		}
	}

	pItem = new class CItem;

	i = iDice(1, 1000);

	if (i < 50) { // Hp slate
		iSlateType = 1;
		cSlateColour = 32;
	}
	else if (i >= 50 && i < 250) { // Bezerk slate
		iSlateType = 2;
		cSlateColour = 3;
	}
	else if (i >= 250 && i < 750) { // Exp slate
		iSlateType = 4;
		cSlateColour = 7;
	}
	else if (i >= 750 && i < 950) { // Mana slate
		iSlateType = 3;
		cSlateColour = 37;
	}
	else if (i >= 950) { // Hp slate
		iSlateType = 1;
		cSlateColour = 32;
	}

	// Notify client
	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATESUCCESS, iSlateType, NULL, NULL, NULL);

	ZeroMemory(cData, sizeof(cData));

	// Create slates
	if (_bInitItemAttr(pItem, 867) == FALSE) {
		delete pItem;
		return;
	}
	else {
		pItem->m_sTouchEffectType = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = iDice(1, 100000);
		pItem->m_sTouchEffectValue2 = iDice(1, 100000);
		pItem->m_sTouchEffectValue3 = (short)timeGetTime();

		_bItemLog(DEF_ITEMLOG_GET, iClientH, -1, pItem);

		pItem->m_sItemSpecEffectValue2 = iSlateType;
		pItem->m_cItemColor = cSlateColour;
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
			ZeroMemory(cData, sizeof(cData));
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMOBTAINED;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0; // 얻은 아이템이므로 장착되지 않았다.
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			*cp = (char)pItem->m_sItemSpecEffectValue2;
			cp++;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			if (iEraseReq == 1) delete pItem;

			// 아이템 정보 전송 
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 메시지를 보낼때 에러가 발생했다면 제거한다.
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
		}
		else {
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
			SendEventToNearClient_TypeB(MSGID_MAGICCONFIGURATIONCONTENTS, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame,
				pItem->m_cItemColor);
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 메시지를 보낼때 에러가 발생했다면 제거한다.
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
		}
	}
}

void CGame::SetSlateFlag(int iClientH, short sType, bool bFlag)
{
	if (m_pClientList[iClientH] == NULL) return;

	if (sType == DEF_NOTIFY_SLATECLEAR) {
		m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
		m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
		m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
		return;
	}

	if (bFlag == TRUE) {
		if (sType == 1) { // Invincible slate
			m_pClientList[iClientH]->m_iStatus |= 0x400000;
		}
		else if (sType == 3) { // Mana slate
			m_pClientList[iClientH]->m_iStatus |= 0x800000;
		}
		else if (sType == 4) { // Exp slate
			m_pClientList[iClientH]->m_iStatus |= 0x10000;
		}
	}
	else {
		if ((m_pClientList[iClientH]->m_iStatus & 0x400000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
		}
		else if ((m_pClientList[iClientH]->m_iStatus & 0x800000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
		}
		else if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
		}
	}

	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
}

// v2.16 2002-5-21 째챠짹짚횉철 횄횩째징
BOOL CGame::bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, BOOL bBonus)
{
	int iValue, iProb;

	if (m_pClientList[iClientH]->m_pItemList[iSomH] == NULL) return FALSE;
	iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x0F0000000) >> 28;

	switch (iValue) {
	case 0: iProb = 30; break;  // +1 :90%     +1~+2
	case 1: iProb = 25; break;  // +2 :80%      +3
	case 2: iProb = 20; break;  // +3 :48%      +4 
	case 3: iProb = 15; break;  // +4 :24%      +5
	case 4: iProb = 10; break;  // +5 :9.6%     +6
	case 5: iProb = 10; break;  // +6 :2.8%     +7
	case 6: iProb = 8; break;  // +7 :0.57%    +8
	case 7: iProb = 8; break;  // +8 :0.05%    +9
	case 8: iProb = 5; break;  // +9 :0.004%   +10
	case 9: iProb = 3; break;  // +10:0.00016%
	default: iProb = 1; break;
	}

	if (((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 > 100)) {
		if (iProb > 20)
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 10);
		else if (iProb > 7)
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 20);
		else
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 40);
	}
	if (bBonus == TRUE) iProb *= 2;

	iProb *= 100;


	if (iProb >= iDice(1, 10000)) {
		_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		return TRUE;
	}

	_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

	return FALSE;
}

/*********************************************************************************************************************
**  BOOL CGame::_bDecodeItemConfigFileContents(char * pData, DWORD dwMsgSize)										**
**  DESCRIPTION			:: decodes Item.cfg file																	**
**  LAST_UPDATED		:: March 17, 2005; 2:09 PM; Hypnotoad														**
**	RETURN_VALUE		:: BOOL																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
BOOL CGame::_bDecodeItemConfigFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iItemConfigListIndex, iTemp;
	class CStrTok* pStrTok;

	pContents = new char[dwMsgSize + 1];
	ZeroMemory(pContents, dwMsgSize + 1);
	memcpy(pContents, pData, dwMsgSize);
	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();
	while (token != NULL) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					// m_sIDnum
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemIDnumber");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					iItemConfigListIndex = atoi(token);
					if (m_pItemConfigList[iItemConfigListIndex] != NULL) {
						wsprintf(cTxt, "(!!!) CRITICAL ERROR! Duplicate ItemIDnum(%d)", iItemConfigListIndex);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex] = new class CItem;
					m_pItemConfigList[iItemConfigListIndex]->m_sIDnum = iItemConfigListIndex;
					cReadModeB = 2;
					break;
				case 2:
					// m_cName 
					ZeroMemory(m_pItemConfigList[iItemConfigListIndex]->m_cName, sizeof(m_pItemConfigList[iItemConfigListIndex]->m_cName));
					memcpy(m_pItemConfigList[iItemConfigListIndex]->m_cName, token, strlen(token));
					cReadModeB = 3;
					break;
				case 3:
					// m_cItemType
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemType");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cItemType = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					// m_cEquipPos
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - EquipPos");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cEquipPos = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					// m_sItemEffectType
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectType");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectType = atoi(token);
					cReadModeB = 6;
					break;
				case 6:
					// m_sItemEffectValue1
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue1");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue1 = atoi(token);
					cReadModeB = 7;
					break;
				case 7:
					// m_sItemEffectValue2
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue2");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue2 = atoi(token);
					cReadModeB = 8;
					break;
				case 8:
					// m_sItemEffectValue3
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue3");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue3 = atoi(token);
					cReadModeB = 9;
					break;
				case 9:
					// m_sItemEffectValue4
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue4");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue4 = atoi(token);
					cReadModeB = 10;
					break;
				case 10:
					// m_sItemEffectValue5
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue5");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue5 = atoi(token);
					cReadModeB = 11;
					break;
				case 11:
					// m_sItemEffectValue6
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue6");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue6 = atoi(token);
					cReadModeB = 12;
					break;
				case 12:
					// m_wMaxLifeSpan
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxLifeSpan");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_wMaxLifeSpan = (WORD)atoi(token);
					cReadModeB = 13;
					break;
				case 13:
					// m_sSpecialEffect
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxFixCount");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffect = atoi(token);
					cReadModeB = 14;
					break;
				case 14:
					// m_sSprite
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Sprite");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSprite = atoi(token);
					cReadModeB = 15;
					break;
				case 15:
					// m_sSpriteFrame
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - SpriteFrame");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpriteFrame = atoi(token);
					cReadModeB = 16;
					break;
				case 16:
					// m_wPrice
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Price");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = FALSE;
					else m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = TRUE;

					m_pItemConfigList[iItemConfigListIndex]->m_wPrice = abs(iTemp);
					cReadModeB = 17;
					break;
				case 17:
					// m_wWeight
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Weight");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_wWeight = atoi(token);
					cReadModeB = 18;
					break;
				case 18:
					// Appr Value
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ApprValue");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cApprValue = atoi(token);
					cReadModeB = 19;
					break;
				case 19:
					// m_cSpeed
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Speed");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cSpeed = atoi(token);
					cReadModeB = 20;
					break;

				case 20:
					// m_sLevelLimit
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - LevelLimit");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sLevelLimit = atoi(token);
					cReadModeB = 21;
					break;

				case 21:
					// m_cGederLimit
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - GenderLimit");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cGenderLimit = atoi(token);
					cReadModeB = 22;
					break;

				case 22:
					// m_sSpecialEffectValue1
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - SM_HitRatio");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue1 = atoi(token);
					cReadModeB = 23;
					break;

				case 23:
					// m_sSpecialEffectValue2
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - L_HitRatio");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue2 = atoi(token);
					cReadModeB = 24;
					break;

				case 24:
					// m_sRelatedSkill
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - RelatedSkill");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sRelatedSkill = atoi(token);
					cReadModeB = 25;
					break;

				case 25:
					// m_cCategory
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cCategory = atoi(token);
					cReadModeB = 26;
					break;

				case 26:
					// m_cItemColor
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cItemColor = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "Item", 4) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}
			if (memcmp(token, "[ENDITEMLIST]", 13) == 0) {
				cReadModeA = 0;
				cReadModeB = 0;
				break;
			}
		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;
	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file contents error!");
		return FALSE;
	}
	wsprintf(cTxt, "(!) ITEM(Total:%d) configuration - success!", iItemConfigListIndex);
	PutLogList(cTxt);
	m_bReceivedItemList = TRUE;
	return TRUE;
}

/*********************************************************************************************************************
**  BOOL CGame::_bInitItemAttr(class CItem * pItem, char * pItemName)												**
**  DESCRIPTION			:: initializes item variables - by ItemName													**
**  LAST_UPDATED		:: March 17, 2005; 2:10 PM; Hypnotoad														**
**	RETURN_VALUE		:: BOOL																						**
**  NOTES				::	- overloaded function - see other _bInitItemAttr										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
BOOL CGame::_bInitItemAttr(class CItem* pItem, char* pItemName)
{
	int i;
	char cTmpName[22];

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pItemName);

	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		if (m_pItemConfigList[i] != NULL) {
			if (memcmp(cTmpName, m_pItemConfigList[i]->m_cName, 20) == 0) {
				ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
				strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
				pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
				pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
				pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
				pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
				pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
				pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
				pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
				pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
				pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
				pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
				pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;
				pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
				pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
				pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
				pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
				pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
				pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
				pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
				pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;
				pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
				pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;
				pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
				pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
				pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;
				pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
				pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
				return TRUE;
			}
		}
	return FALSE;
}

/*********************************************************************************************************************
**  BOOL CGame::_bInitItemAttr(class CItem * pItem, char * pItemName)												**
**  DESCRIPTION			:: initializes item variables - by ItemID													**
**  LAST_UPDATED		:: March 17, 2005; 2:10 PM; Hypnotoad														**
**	RETURN_VALUE		:: BOOL																						**
**  NOTES				::	- overloaded function - see other _bInitItemAttr										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
BOOL CGame::_bInitItemAttr(class CItem* pItem, int iItemID)
{
	int i;

	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		if (m_pItemConfigList[i] != NULL) {
			if (m_pItemConfigList[i]->m_sIDnum == iItemID) {
				ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
				strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
				pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
				pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
				pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
				pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
				pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
				pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
				pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
				pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
				pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
				pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
				pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;
				pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
				pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
				pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
				pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
				pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
				pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
				pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
				pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;
				pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
				pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;
				pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
				pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
				pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;
				pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
				pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
				return TRUE;
			}
		}
	return FALSE;
}

int CGame::_iGetItemSpaceLeft(int iClientH)
{
	int i, iTotalItem;

	iTotalItem = 0;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != NULL) iTotalItem++;

	return (DEF_MAXITEMS - iTotalItem);
}

BOOL CGame::bAddItem(int iClientH, CItem* pItem, char cMode)
{
	char* cp, cData[256];
	DWORD* dwp;
	WORD* wp;
	short* sp;
	int iRet, iEraseReq;


	ZeroMemory(cData, sizeof(cData));
	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
		// 쩐횈��횑횇횤��쨩 횊쨔쨉챈횉횩쨈횢.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		// 1째쨀 횊쨔쨉챈횉횩쨈횢. Amount째징 쩐횈쨈횕쨈횢!
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;	// 쩌철쨌짰��쨩 ��횚쨌횂 
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;


		if (iEraseReq == 1) {
			delete pItem;
			pItem = NULL;
		}

		// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

		return TRUE;
	}
	else {
		// 쩐횈��횑횇횤��쨩 ��체쨈횧쨔횧��쨘 횆쨀쨍짱횇횒째징 쨈천��횑쨩처 쩐횈��횑횇횤��쨩 쨘쨍째체횉횘 쩌철 쩐첩쨈횂 쨩처횇횂��횑쨈횢.
		// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY,
			pItem);

		// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

// 쨈천��횑쨩처 째징횁첬쩌철 쩐첩쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);

		return TRUE;
	}

	return FALSE;
}

void CGame::SendItemNotifyMsg(int iClientH, WORD wMsgType, CItem* pItem, int iV1)
{
	char* cp, cData[512];
	DWORD* dwp;
	WORD* wp;
	short* sp;
	int     iRet;

	if (m_pClientList[iClientH] == NULL) return;

	dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_NOTIFY;
	wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;
	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	switch (wMsgType) {
	case DEF_NOTIFY_ITEMOBTAINED:
		*cp = 1; // 1째쨀 횊쨔쨉챈횉횩쨈횢. <- 쩔짤짹창쩌짯 1째쨀쨋천 쩌철쨌짰 횆짬쩔챤횈짰쨍짝 쨍쨩횉횕쨈횂 째횒��횑 쩐횈쨈횕쨈횢
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor; // v1.4
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;


		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);
		break;

	case DEF_NOTIFY_ITEMPURCHASED:
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		wp = (WORD*)cp;
		*wp = iV1;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 48);
		break;

	case DEF_NOTIFY_CANNOTCARRYMOREITEM:
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		break;
	}
}

BOOL CGame::_bCheckItemReceiveCondition(int iClientH, CItem* pItem)
{
	int i;

	if (m_pClientList[iClientH] == NULL) return FALSE;


	if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount) > (DWORD)_iCalcMaxLoad(iClientH))
		return FALSE;

	// 쩐횈��횑횇횤��쨩 쨔횧��쨩 쩔짤��짱째첩째짙 ��짱쨔짬 횈횉쨈횥.
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == NULL) return TRUE;

	return FALSE;
}

// New 07/05/2004
void CGame::DropItemHandler(int iClientH, short sItemIndex, int iAmount, char* pItemName, BOOL bByPlayer)
{
	class CItem* pItem;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
	if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
	if ((iAmount != -1) && (iAmount < 0)) return;

	// Amount째징 -1��횑째챠 쩌횘쨍챨횉째��횑쨍챕 쩌철쨌짰��횉 ��체쨘횓쨍짝 쨋쨀쩐챤쨋횩쨍째쨈횢.
	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(iAmount == -1))
		iAmount = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount;


	// 쩐횈��횑횇횤 ��횑쨍짠��횑 ��횕횆징횉횕횁철 쩐횎쩐횈쨉쨉 쨔짬쩍횄 
	if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) return;

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(((int)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount - iAmount) > 0)) {
		// 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��횑쩐첬째챠 쩌철쨌짰쨍쨍횇짯 째짢쩌횘쩍횄횇째째챠 쨀짼��쨘 째횚 ��횜쨈횢쨍챕 
		pItem = new class CItem;
		if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == FALSE) {
			// 쨘횖횉횘횉횕째챠��횣 횉횕쨈횂 쩐횈��횑횇횤��횑 쨍짰쩍쨘횈짰쩔징 쩐첩쨈횂 째횇쨈횢. ��횑쨌짹 ��횕��쨘 ��횕쩐챤쨀짱 쩌철째징 쩐첩횁철쨍쨍 
			delete pItem;
			return;
		}
		else {
			if (iAmount <= 0) {
				// 쩐횈��횑횇횤��횉 쩌철쨌짰��횑 0쨘쨍쨈횢 ��횤��쨍쨍챕 쩔징쨌짱쨩처횊짼. 쨍짰횇횕 
				delete pItem;
				return;
			}
			pItem->m_dwCount = (DWORD)iAmount;
		}

		// 쩌철쨌짰 째짢쩌횘 

		// 쩔징쨌짱. 째짢쩌횘쩍횄횇째째챠��횣 횉횕쨈횂 쩐챌��횑 쨈천 쨍쨔쨈횢. 
		if ((DWORD)iAmount > m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount) {
			delete pItem;
			return;
		}

		m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

		// 쨘짱째챈쨉횊 쩌철쨌짰��쨩 쩌쨀횁짚횉횕째챠 쩐횏쨍째쨈횢.
		// v1.41 !!!
		SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

		// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY, pItem);

		// v1.411 횊챰짹횒 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��횓횁철 횄쩌횇짤  
		// v2.17 2002-7-31 횉횄쨌쨔��횑쩐챤째징 횁횞쩐챤쩌짯 쨋쨀쩐챤횁첩째횉 쨍챨쨉챌 쨌횓짹횞째징 쨀짼쨈횂쨈횢. 
		if (bByPlayer == TRUE)
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);
		else
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem, TRUE);

		// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color

		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, sItemIndex, iAmount, NULL, NULL);
	}
	else {
		// ��횕쨔횦 쩐횈��횑횇횤��횑쨀짧 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��쨩 쨍챨쨉횓 쨔철쨌횊쨈횢.

		// 쨍횛��첬 ��책횂첩쨉횉쩐챤 ��횜쨈횢쨍챕 횉횠횁짝쩍횄횇짼쨈횢.

		ReleaseItemHandler(iClientH, sItemIndex, TRUE);

		// v2.17 쩐횈��횑횇횤��횑 ��책횂첩쨉횉쩐챤 ��횜��쨍쨍챕 횉횠횁짝횉횗쨈횢.
		if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == TRUE)
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);

		// v1.432
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) &&
			(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0)) {
			// 횊챰쨩첵��횉 쨉쨔��횓 째챈쩔챙 쩌철쨍챠��횑 0��횓 쨩처횇횂쨌횓 쨋쨀쩐챤횁철쨍챕 쨩챌쨋처횁첩쨈횢.
			delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
			m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
		}
		else {
			// 쩐횈��횑횇횤��쨩 쩌짯��횜쨈횂 ��짠횆징쩔징 쨔철쨍째쨈횢. 
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
				m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]);

			// v1.41 횊챰짹횒 쩐횈��횑횇횤��쨩 쨋쨀쩐챤쨋횩쨍째 째횒��횑쨋처쨍챕 쨌횓짹횞쨍짝 쨀짼짹채쨈횢. 
			// v2.17 2002-7-31 횉횄쨌쨔��횑쩐챤째징 횁횞쩐챤쩌짯 쨋쨀쩐챤횁첩째횉 쨍챨쨉챌 쨌횓짹횞째징 쨀짼쨈횂쨈횢. 
			if (bByPlayer == TRUE)
				_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			else
				_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex], TRUE);

			// 쨈횢쨍짜 횇짭쨋처��횑쩐챨횈짰쩔징째횚 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁첩 째횒��쨩 쩐횏쨍째쨈횢. 
			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); //v1.4 color
		}

		// 쩐횈��횑횇횤��쨩 delete횉횕횁철 쩐횎째챠 NULL쨌횓 횉횘쨈챌횉횗쨈횢. delete 횉횕횁철 쩐횎쨈횂 ��횑��짱쨈횂 쨔횢쨈횣쩔징 쨋쨀쩐챤횁짰 ��횜짹창 쨋짠쨔짰 
		m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
		m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

		// ��횑횁짝 쩐횈��횑횇횤��쨩 쨍짰쩍쨘횈짰쩔징쩌짯 쨩챔횁짝횉횘째횒��쨩 횇챘쨘쨍횉횗쨈횢.
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, NULL);

		// ��횓쨉짝쩍쨘째징 쨔횢짼챤쩐첬��쨍쨔횉쨌횓 ��챌횉횘쨈챌
		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}

	// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
	iCalcTotalWeight(iClientH);
}

/////////////////////////////////////////////////////////////////////////////////////
//  int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
//  description			:: check if player is dropping item or picking up item
//  last updated		:: October 29, 2004; 7:12 PM; Hypnotoad
//	return value		:: int
/////////////////////////////////////////////////////////////////////////////////////
int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
{
	DWORD* dwp;
	WORD* wp;
	char* cp;
	short* sp, sRemainItemSprite, sRemainItemSpriteFrame;
	char  cRemainItemColor, cData[100];
	int   iRet, iEraseReq;
	class CItem* pItem;

	if (m_pClientList[iClientH] == NULL) return 0;
	if ((cDir <= 0) || (cDir > 8))       return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

	if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

	int iStX, iStY;
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != NULL) {
		iStX = m_pClientList[iClientH]->m_sX / 20;
		iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;

		switch (m_pClientList[iClientH]->m_cSide) {
		case 0: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
		}
	}

	ClearSkillUsingStatus(iClientH);

	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

	pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(sX, sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
	if (pItem != NULL) {
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {

			_bItemLog(DEF_ITEMLOG_GET, iClientH, NULL, pItem);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMOBTAINED;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0;
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			*cp = (char)pItem->m_sItemSpecEffectValue2;
			cp++;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			if (iEraseReq == 1) delete pItem;

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, TRUE, TRUE);
				return 0;
			}
		}
		else
		{
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(sX, sY, pItem);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, TRUE, TRUE);
				return 0;
			}
		}
	}

	dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_MOTION;
	wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = DEF_OBJECTMOTION_CONFIRM;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, TRUE, TRUE);
		return 0;
	}

	return 1;
}

BOOL CGame::_bAddClientItemList(int iClientH, class CItem* pItem, int* pDelReq)
{
	int i;

	if (m_pClientList[iClientH] == NULL) return FALSE;
	if (pItem == NULL) return FALSE;

	// 횁첵��쨩 쩐횈��횑횇횤 횁횩쨌짰째챔쨩챗  
	if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)) {
		// 쩌철쨌짰째쨀쨀채��횑 ��횜쨈횂 쩐횈��횑횇횤 
		if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount)) > (DWORD)_iCalcMaxLoad(iClientH))
			return FALSE;
	}
	else {
		// 쩌철쨌짰 째쨀쨀채��횑 쩐첩쨈횂 쩐횈��횑횇횤 
		if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, 1)) > (DWORD)_iCalcMaxLoad(iClientH))
			return FALSE;
	}

	// 쩌횘쨘챰쩌쨘 쩐횈��횑횇횤��쨩 ��횑쨔횑 쩌횘횁철횉횕째챠 ��횜쨈횢쨍챕 쩌철쨌짰쨍쨍 횁천째징쩍횄횇짼쨈횢. 
	if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)) {
		for (i = 0; i < DEF_MAXITEMS; i++)
			if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) &&
				(memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, pItem->m_cName, 20) == 0)) {
				// 째째��쨘 ��횑쨍짠��쨩 횄짙쩐횘쨈횢. 
				m_pClientList[iClientH]->m_pItemList[i]->m_dwCount += pItem->m_dwCount;

				*pDelReq = 1;

				// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
				iCalcTotalWeight(iClientH);

				return TRUE;
			}
	}

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == NULL) {

			m_pClientList[iClientH]->m_pItemList[i] = pItem;
			// v1.3 쩐횈��횑횇횤��횉 ��짠횆징쨈횂 쨔짬횁쨋째횉 100, 100
			m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
			m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

			*pDelReq = 0;

			// 쨍쨍쩐횪 횊짯쨩챙쨌첫 쩐횈��횑횇횤��횑쨋처쨍챕 횊짯쨩챙��쨩 횉횘쨈챌횉횗쨈횢. 
			if (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)
				m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

			// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
			iCalcTotalWeight(iClientH);

			return TRUE;
		}

	// 쨈천��횑쨩처 쩐횈��횑횇횤��쨩 횁첵��쨩 째첩째짙��횑 쩐첩쨈횢.
	return FALSE;
}

BOOL CGame::bEquipItemHandler(int iClientH, short sItemIndex, BOOL bNotify)
{
	char  cEquipPos, cHeroArmorType;
	short   sSpeed;
	short sTemp;
	int iTemp;

	if (m_pClientList[iClientH] == NULL) return FALSE;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return FALSE;

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) return FALSE;

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) == NULL) &&
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sLevelLimit > m_pClientList[iClientH]->m_iLevel)) return FALSE;


	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 0) {
		switch (m_pClientList[iClientH]->m_sType) {
		case 1:
		case 2:
		case 3:
			// 쨀짼쩌쨘��횑쨈횢.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 1) return FALSE;
			break;
		case 4:
		case 5:
		case 6:
			// 쩔짤쩌쨘��횑쨈횢.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 2) return FALSE;
			break;
		}
	}

	if (iGetItemWeight(m_pClientList[iClientH]->m_pItemList[sItemIndex], 1) > (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) * 100) return FALSE;

	cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

	if ((cEquipPos == DEF_EQUIPPOS_BODY) || (cEquipPos == DEF_EQUIPPOS_LEGGINGS) ||
		(cEquipPos == DEF_EQUIPPOS_ARMS) || (cEquipPos == DEF_EQUIPPOS_HEAD)) {
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4) {
		case 10: // Str 횁짝횉횗 
			if ((m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		case 11: // Dex
			if ((m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iAngelicDex) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		case 12: // Vit
			if (m_pClientList[iClientH]->m_iVit < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		case 13: // Int
			if ((m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iAngelicInt) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		case 14: // Mag
			if ((m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iAngelicMag) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		case 15: // Chr
			if (m_pClientList[iClientH]->m_iCharisma < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5) {
				// 횇짭쨋처��횑쩐챨횈짰 쨩처쩔징쩌짯쨈횂 횂첩쩔챘쨉횊 쨩처횇횂��횑쨔횉쨌횓 쨔첸째횥쩐횩 횉횗쨈횢. 횂첩쩔챘��횑 횉횠횁짝쨉횊쨈횢.
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
				return FALSE;
			}
			break;
		}
	}

	if (cEquipPos == DEF_EQUIPPOS_TWOHAND) {
		// Stormbringer
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 845) {
			if ((m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iAngelicInt) < 65) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, sItemIndex, NULL, NULL);
				ReleaseItemHandler(iClientH, sItemIndex, TRUE);
				return FALSE;
			}
			else
			{
				m_pClientList[iClientH]->m_iHitRatio += 10;
			}
		}
		// Centuu : Fixed las armas Blood by KaoZureS
		else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 490) { // Sword
			if (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr < 131) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, sItemIndex, NULL, NULL);
				ReleaseItemHandler(iClientH, sItemIndex, TRUE);
				return FALSE;
			}
			else
			{
				m_pClientList[iClientH]->m_iHitRatio += 10;
			}
		}
	}

	if (cEquipPos == DEF_EQUIPPOS_RHAND) 
	{
		// Resurrection wand(MS.10) or Resurrection wand(MS.20)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866)) {
			if (((m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iAngelicInt) > 99) && ((m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iAngelicMag) > 99) && (m_pClientList[iClientH]->m_iSpecialAbilityTime < 1)) {
				m_pClientList[iClientH]->m_cMagicMastery[94] = TRUE;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESUR_ON, NULL, NULL, NULL, NULL);
			}
		}
		// Centuu : Fixed las armas Blood by KaoZureS
		else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 491) { // Axe
			if (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr < 61) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, sItemIndex, NULL, NULL);
				ReleaseItemHandler(iClientH, sItemIndex, TRUE);
				return FALSE;
			}
			else
			{
				m_pClientList[iClientH]->m_iHitRatio += 10;
			}
		}
		// Centuu : Fixed las armas Blood by KaoZureS
		else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 492) { // Rapier
			if (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr < 11) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, sItemIndex, NULL, NULL);
				ReleaseItemHandler(iClientH, sItemIndex, TRUE);
				return FALSE;
			}
			else
			{
				m_pClientList[iClientH]->m_iHitRatio += 10;
			}
		}
	}

	if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)) {

		if ((m_pClientList[iClientH]->m_iSpecialAbilityType != 0)) {
			// ��횑쨔횑 횈짱쩌철쨈횋쨌횂��횑 쩌쨀횁짚쨉횉쩐챤 ��횜쨈횢. ��횑��체쩔징 ��책횂첩쨉횉쩐챤 ��횜쨈횂 쩐횈��횑횇횤 횂첩쩔챘 횉횠횁짝:
			// 쨈횥, 횂첩쩔챘 쨘횓��짠째징 째째쨈횢쨍챕 횉횠횁짝 쨍횧쩍횄횁철 쨘쨍쨀쨩횁철 쩐횎쨈횂쨈횢.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos != m_pClientList[iClientH]->m_iSpecialAbilityEquipPos) {
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], NULL, NULL);
				// 횉횠쨈챌 횂첩쩔챘 쨘횓��짠��횉 쩐횈��횑횇횤횊쩔째첬쨍짝 횁짝째횇.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], TRUE);
			}
		}
	}


	if (cEquipPos == DEF_EQUIPPOS_NONE) return FALSE;

	if (cEquipPos == DEF_EQUIPPOS_TWOHAND) {
		// 쩐챌쩌횛��쨩 쨩챌쩔챘횉횕쨈횂 쨔짬짹창째징 ��책횂첩쨉횉쩐챤 ��횜쨈횢쨍챕  
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
		else {
			// 쩔��쨍짜쩌횛, 쩔횧쩌횛쩔징 쩐횈��횑횇횤��횑 ��횜쨈횢쨍챕 쨍챨쨉횓 ��책횂첩 횉횠횁짝 
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND], FALSE);
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND], FALSE);
		}
	}
	else {
		// 쩐챌쩌횛 쩐횈��횑횇횤��횑 ��책횂첩쨉횉쩐챤 ��횜쨈횢쨍챕 횉횠횁짝쩍횄횇짼쨈횢.
		if ((cEquipPos == DEF_EQUIPPOS_LHAND) || (cEquipPos == DEF_EQUIPPOS_RHAND)) {
			// 쨔짬짹창쨀짧 쨔챈횈횖쨌첫쨋처쨍챕 쩐챌쩌횛쨔짬짹창쨍짝 횉횠횁짝 
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND], FALSE);
		}

		// ��책횂첩��짠횆징쩔징 쩐횈��횑횇횤��횑 ��횑쨔횑 ��횜��쨍쨍챕 쨩챔횁짝.
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
	}


	if (cEquipPos == DEF_EQUIPPOS_RELEASEALL) {
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS], FALSE);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK], FALSE);
		}
	}
	else {
		if (cEquipPos == DEF_EQUIPPOS_HEAD || cEquipPos == DEF_EQUIPPOS_BODY || cEquipPos == DEF_EQUIPPOS_ARMS ||
			cEquipPos == DEF_EQUIPPOS_LEGGINGS || cEquipPos == DEF_EQUIPPOS_PANTS || cEquipPos == DEF_EQUIPPOS_BACK) {
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL] != -1) {
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL], FALSE);
			}
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
	}


	m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = sItemIndex;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = TRUE;

	switch (cEquipPos) {

	case DEF_EQUIPPOS_HEAD:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFF0F;	// 횇천짹쨍 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFFF0; // 쨘횓횄첨 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.				
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor));
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_PANTS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xF0FF;	// 쨔횢횁철 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFF0FF; // 쨔횢횁철 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 8);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_LEGGINGS:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0x0FFF;	// 쨘횓횄첨 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFF0F; // 쨘횓횄첨 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 4);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BODY:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 째횗쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.

		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue < 100) {
			// 쩔횥횉체째짧��횑 100 ��횑횉횕. 횁짚쨩처��청��횓 째짧 
			sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 째짧��쨩 쩌쩌횈횄. 
			m_pClientList[iClientH]->m_sAppr3 = sTemp;
		}
		else {
			// 쩔횥횉체째짧��횑 100 ��횑쨩처��횑쨍챕 횊짰��책 횉횄쨌징짹횞쨍짝 쨩챌쩔챘횉횗쨈횢. 
			sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue - 100) << 12); // Appr 째짧��쨩 쩌쩌횈횄. 
			m_pClientList[iClientH]->m_sAppr3 = sTemp;
			// 째횗쩔횎 횊짰��책 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 쩌쩌횈짰.
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp | 0x080;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
		}

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFF0FFFFF; // 째횗쩔횎(째짤쩔횎) 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 20);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_ARMS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFFF0;	// 쩌횙쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue)); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFFFF; // 쩌횙쩔횎 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤. // New 09/05/2004
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 12);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_LHAND:
		// 쩔횧쩌횛쩔징 ��책횂첩횉횕쨈횂 쩐횈��횑횇횤. 쨔챈횈횖쨌첫��횑쨈횢. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xFFF0;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue)); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xF0FFFFFF; // 쨔챈횈횖 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 24);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RHAND:
		// 쩔��쨍짜쩌횛쩔징 ��책횂첩횉횕쨈횂 쩐횈��횑횇횤. 쨔짬짹창쨌첫��횑쨈횢. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFFF; // 쨔짬짹창 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// 쨔짬짹창 쩌횙쨉쨉쨍짝 쨀짧횇쨍쨀쨩쨈횂 Status��횉 쨘챰횈짰쨍짝 쩌쨀횁짚횉횗쨈횢.
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
		// Str쩔징 쨉청쨋처 쨔짬짹창 쩌횙쨉쨉쨍짝 횁횢��횓쨈횢. 
		sSpeed -= ((m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) / 13);
		if (sSpeed < 0) sSpeed = 0;
		iTemp = iTemp | (int)sSpeed;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		m_pClientList[iClientH]->m_iComboAttackCount = 0;
		break;

	case DEF_EQUIPPOS_TWOHAND:
		// 쩐챌쩌횛횇쨍��횚��횑횁철쨍쨍 쨩챌쩍횉 쩔��쨍짜쩌횛쩔징 ��책횂첩쨉횊쨈횢. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFFF; // 쨔짬짹창 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// 쨔짬짹창 쩌횙쨉쨉쨍짝 쨀짧횇쨍쨀쨩쨈횂 Status��횉 쨘챰횈짰쨍짝 쩌쨀횁짚횉횗쨈횢.
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
		// Str쩔징 쨉청쨋처 쨔짬짹창 쩌횙쨉쨉쨍짝 횁횢��횓쨈횢. 
		sSpeed -= ((m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) / 13);
		if (sSpeed < 0) sSpeed = 0;
		iTemp = iTemp | (int)sSpeed;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		m_pClientList[iClientH]->m_iComboAttackCount = 0;
		break;

	case DEF_EQUIPPOS_BACK:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xF0FF;	// 쨍횁횇채 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFF0FFFF; // 쨍횁횇채 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 16);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RELEASEALL:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 쨍횁횇채 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 째짧��쨩 쩌쩌횈횄. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFF0FFFF; // 쨍횁횇채 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) {
		// 째첩째횦 횈짱쩌철 쨈횋쨌횂 쨘챰횈짰 횇짭쨍짰쩐챤
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect) {
		case 0: break;
		case 1: // 째첩째횦쩍횄 50% HP 째짢쩌횘
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0004;
			break;

		case 2: // 쨀횄쨉쩔 횊쩔째첬 
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x000C;
			break;

		case 3: // 쨍쨋쨘챰 횊쩔째첬 
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0008;
			break;
		}
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY) {
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect) {
		case 0:
			break;
		case 50:
		case 51:
		case 52:
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0002;
			break;
		default:
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0001;

			break;
		}
	}

	cHeroArmorType = _cCheckHeroItemEquipped(iClientH);
	if (cHeroArmorType != 0x0FFFFFFFF) m_pClientList[iClientH]->m_cHeroArmourBonus = cHeroArmorType;

	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
	CalcTotalItemEffect(iClientH, sItemIndex, bNotify);
	return TRUE;

}

void CGame::ReleaseItemHandler(int iClientH, short sItemIndex, BOOL bNotice)
{
	char cEquipPos, cHeroArmorType;
	short  sTemp;
	int   iTemp;

	if (m_pClientList[iClientH] == NULL) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return;

	if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == FALSE) return;

	cHeroArmorType = _cCheckHeroItemEquipped(iClientH);
	if (cHeroArmorType != 0x0FFFFFFFF) m_pClientList[iClientH]->m_cHeroArmourBonus = 0;

	cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;
	if (cEquipPos == DEF_EQUIPPOS_RHAND) {
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) {
			if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866)) {
				m_pClientList[iClientH]->m_cMagicMastery[94] = FALSE;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESUR_OFF, NULL, NULL, NULL, NULL);
			}
		}
	}
	// 쩐횈��횑횇횤��횉 횉횠횁짝��짠횆징쩔징 쨍횂째횚 Appr쨘짱쩌철쨍짝 횁쨋횁짚횉횗쨈횢.
	switch (cEquipPos) {
	case DEF_EQUIPPOS_RHAND:
		// 쩔��쨍짜쩌횛쩔징 ��책횂첩횉횕쨈횂 쩐횈��횑횇횤. 쨔짬짹창쨌첫��횑쨈횢. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0x0FFFFFFF; // 쨔짬짹창 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// V1.3 쨔짬짹창쩌횙쨉쨉 ��챌째챔쨩챗 <- 쨍횉쩌횛��횑쨈횕짹챰 0
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		break;

	case DEF_EQUIPPOS_LHAND:
		// 쩔횧쩌횛쩔징 ��책횂첩횉횕쨈횂 쩐횈��횑횇횤. 쨔챈횈횖쨌첫��횑쨈횢. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xFFF0;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xF0FFFFFF; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_TWOHAND:
		// 쩐챌쩌횛쩔징 ��책횂첩횉횗쨈횢횁철쨍쨍 쨩챌쩍횉 쩔��쨍짜쩌횛쩔징쨍쨍 쨉챕째챠 ��횜쨈횢. 쩔횧쩌횛��쨘 쨘챰쩐챤��횜쨈횂 쨩처횇횂.
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 쨔짬짹창 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0x0FFFFFFF; // 쨔짬짹창 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BODY:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 째횗쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		// 째횗쩔횎 횊짰��책 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xFF7F;
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFF0FFFFF; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BACK:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xF0FF;	// 쨍횁횇채 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFF0FFFF; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_ARMS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFFF0;	// 째횗쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFF0FFF; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_PANTS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xF0FF;	// 째횗쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFF0FF; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_LEGGINGS:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0x0FFF;	// 째횗쩔횎 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFF0F; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_HEAD:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFF0F;	// 횇천짹쨍 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFFFFFF0; // 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RELEASEALL:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 쨍횁횇채 횈짱쩌쨘횆징 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 횆횄쨌짱째짧 쩌쩌횈횄 
		iTemp = iTemp & 0xFFF0FFFF; // 쨍횁횇채 쨩철 쨘챰횈짰쨍짝 횇짭쨍짰쩐챤.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;
	}

	//v1.432 횈짱쩌철 쨈횋쨌횂��횑 쨘횓쩔짤쨉횊 쩐횈��횑횇횤��횑쨋처쨍챕 횉횄쨌징짹횞 쩌쨀횁짚 
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) {
		// 째첩째횦 횈짱쩌철 쨈횋쨌횂 쨘챰횈짰 횇짭쨍짰쩐챤
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY) {
		// 쨔챈쩐챤짹쨍 횈짱쩌철 쨈횋쨌횂 쨘챰횈짰 횇짭쨍짰쩐챤
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
	}

	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;
	m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = -1;

	// 쩔횥횉체��횑 쨔횢짼챦째횒��쨩 쩐횏쨍째쨈횢.
	if (bNotice == TRUE)
		SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

	// ��횑 쩐횈��횑횇횤��쨩 횉횠횁짝횉횕쨈횂 째횒쩔징 쨉청쨍짙쨈횂 횈짱쩌쨘횆징��횉 쨘짱횊짯쨍짝 째챔쨩챗횉횗쨈횢. 
	CalcTotalItemEffect(iClientH, sItemIndex, TRUE);
}

BOOL CGame::bPlayerItemToBank(int iClientH, short sItemIndex)
{
	int i, iIndex;

	if (m_pClientList[iClientH] == NULL) return FALSE;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL) {
			iIndex = i;
			goto NEXT_STEP_PLTB;
		}
	// 쨈천��횑쨩처 ��첬��책횉횘 째첩째짙��횑 쩐첩쨈횢. 
	return FALSE;

NEXT_STEP_PLTB:;

	// 쩐횈��횑횇횤��쨩 ��첬��책횉횘 째첩째짙��횑 쨀짼쩐횈��횜쨈횢. 
	// 쨍횛��첬 ��책횂첩쨉횉쩐챤 ��횜쨈횢쨍챕 횉횠횁짝쩍횄횇짼쨈횢.
	ReleaseItemHandler(iClientH, sItemIndex, TRUE);

	// 쩐횈��횑횇횤 횇짭쨌징쩍쨘��횉 횁횜쩌횘쨍짝 쨔횢짼횤쨈횢. 
	m_pClientList[iClientH]->m_pItemInBankList[iIndex] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
	// 횉횄쨌쨔��횑쩐챤 쩐횈��횑횇횤 쨍짰쩍쨘횈짰쨍짝 횇짭쨍짰쩐챤횉횕째챠 
	m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

	// 쩐횈��횑횇횤 쨍짰쩍쨘횈짰��횉 쨘처 째첩째짙��쨩 쨩챔횁짝횉횗쨈횢.
	for (i = 1; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i - 1] == NULL) && (m_pClientList[iClientH]->m_pItemList[i] != NULL)) {
			m_pClientList[iClientH]->m_pItemList[i - 1] = m_pClientList[iClientH]->m_pItemList[i];
			m_pClientList[iClientH]->m_bIsItemEquipped[i - 1] = m_pClientList[iClientH]->m_bIsItemEquipped[i];
			m_pClientList[iClientH]->m_pItemList[i] = NULL;
			m_pClientList[iClientH]->m_bIsItemEquipped[i] = FALSE;
		}

	return TRUE;
}

BOOL CGame::bBankItemToPlayer(int iClientH, short sItemIndex)
{
	int i, iIndex;

	if (m_pClientList[iClientH] == NULL) return FALSE;

	if (m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] == NULL) return FALSE;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == NULL) {
			iIndex = i;
			goto NEXT_STEP_PLTB;
		}
	// 쨈천��횑쨩처 째짰째챠��횜��쨩 째첩째짙��횑 쩐첩쨈횢. 
	return FALSE;

NEXT_STEP_PLTB:;

	// 쩐횈��횑횇횤��쨩 쩌횘횁철횉횘 째첩째짙��횑 쨀짼쩐횈��횜쨈횢. 

	// 쩐횈��횑횇횤 횇짭쨌징쩍쨘��횉 횁횜쩌횘쨍짝 쨔횢짼횤쨈횢. 
	m_pClientList[iClientH]->m_pItemList[iIndex] = m_pClientList[iClientH]->m_pItemInBankList[sItemIndex];

	m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] = NULL;

	return TRUE;
}

void CGame::RequestRetrieveItemHandler(int iClientH, char* pData)
{
	char* cp, cBankItemIndex, cMsg[100];
	int i, j, iRet, iItemWeight;
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cBankItemIndex = *cp;

	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return;

	if ((cBankItemIndex < 0) || (cBankItemIndex >= DEF_MAXBANKITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] == NULL) {
		// 쩔��쨌첫쨈횢. 
		ZeroMemory(cMsg, sizeof(cMsg));

		dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_RETRIEVEITEM;
		wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
		*wp = DEF_MSGTYPE_REJECT;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
	}
	else {
		// 횁횩쨌짰째챔쨩챗 
		iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

		if ((iItemWeight + m_pClientList[iClientH]->m_iCurWeightLoad) > _iCalcMaxLoad(iClientH)) {
			// 횉횗째챔횁횩쨌짰 횄횎째첬, 쩐횈��횑횇횤��쨩 횄짙��쨩 쩌철 쩐첩쨈횢. 
			// 쩍횉횈횖 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
			ZeroMemory(cMsg, sizeof(cMsg));

			// 쨈천��횑쨩처 째징횁첬쩌철 쩐첩쨈횢쨈횂 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
			dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
				DeleteClient(iClientH, TRUE, TRUE);
				break;
			}
			return;
		}

		//!!!
		if ((m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
			// 횁횩쨘쨔��횑 째징쨈횋횉횗 쩐횈��횑횇횤��횑쨋처쨍챕 쩌철쨌짰쨍쨍 횁천째징쩍횄횇짼쨈횢.	
			for (i = 0; i < DEF_MAXITEMS; i++)
				if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType) &&
					(memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cName, 20) == 0)) {
					// 째째��쨘 횉체쩍횆��횉 쩐횈��횑횇횤��쨩 횄짙쩐횘쨈횢. 쩌철쨌짰��쨩 횁천째징쩍횄횇짼쨈횢.
					// v1.41 !!! 
					SetItemCount(iClientH, i, m_pClientList[iClientH]->m_pItemList[i]->m_dwCount + m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

					// 쨔챨횇짤 쩐횈��횑횇횤 쨩챔횁짝 
					delete m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
					m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = NULL;

					// 쨘처 째첩째짙��쨩 쩐첩쩐횠쨈횢. 
					for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++) {
						if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != NULL) && (m_pClientList[iClientH]->m_pItemInBankList[j] == NULL)) {
							m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

							m_pClientList[iClientH]->m_pItemInBankList[j + 1] = NULL;
						}
					}

					// 쩌쨘째첩 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
					ZeroMemory(cMsg, sizeof(cMsg));

					dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
					*dwp = MSGID_RESPONSE_RETRIEVEITEM;
					wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
					*wp = DEF_MSGTYPE_CONFIRM;

					cp = (char*)(cMsg + DEF_INDEX2_MSGTYPE + 2);
					*cp = cBankItemIndex;
					cp++;
					*cp = i;
					cp++;

					// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
					iCalcTotalWeight(iClientH);
					// 횊짯쨩챙 횉횘쨈챌
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					// 쨍횧쩍횄횁철 ��체쩌횤 
					iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(iClientH, TRUE, TRUE);
						break;
					}
					return;
				}

			// 째째��쨘 ��횑쨍짠��쨩 째짰째챠 ��횜쨈횂 쩐횈��횑횇횤��횑 쩐첩쨈횢. 쨩천쨌횓 횄횩째징횉횠쩐횩 횉횗쨈횢. 
			goto RRIH_NOQUANTITY;
		}
		else {
		RRIH_NOQUANTITY:;
			// 쩌철쨌짰째쨀쨀채��횑 쩐첩쨈횂 쩐횈��횑횇횤 
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] == NULL) {
					// 쨘처 째첩째짙��쨩 횄짙쩐횘쨈횢. 
					// 쨍횛��첬 횁횜쩌횘쨍짝 쩔횇짹채쨈횢. 
					m_pClientList[iClientH]->m_pItemList[i] = m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
					// v1.3 1-27 12:22
					m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
					m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

					m_pClientList[iClientH]->m_bIsItemEquipped[i] = FALSE;

					m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = NULL;

					// 쨘처 째첩째짙��쨩 쩐첩쩐횠쨈횢. 
					for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++) {
						if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != NULL) && (m_pClientList[iClientH]->m_pItemInBankList[j] == NULL)) {
							m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

							m_pClientList[iClientH]->m_pItemInBankList[j + 1] = NULL;
						}
					}

					// 쩌쨘째첩 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩍쨈횢.
					ZeroMemory(cMsg, sizeof(cMsg));

					dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
					*dwp = MSGID_RESPONSE_RETRIEVEITEM;
					wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
					*wp = DEF_MSGTYPE_CONFIRM;

					cp = (char*)(cMsg + DEF_INDEX2_MSGTYPE + 2);
					*cp = cBankItemIndex;
					cp++;
					*cp = i;
					cp++;

					// 쩌횘횁철횉째 횄횗 횁횩쨌짰 ��챌 째챔쨩챗 
					iCalcTotalWeight(iClientH);

					// 횊짯쨩챙 횉횘쨈챌
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					// 쨍횧쩍횄횁철 ��체쩌횤 
					iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
						DeleteClient(iClientH, TRUE, TRUE);
						break;
					}
					return;
				}
			// 쩐횈��횑횇횤��쨩 쨉횉횄짙��쨩 째첩째짙��횑 쩐첩쨈횢. 쩔��쨌첫
			ZeroMemory(cMsg, sizeof(cMsg));

			dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
			*dwp = MSGID_RESPONSE_RETRIEVEITEM;
			wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
		}
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
		DeleteClient(iClientH, TRUE, TRUE);
		break;
	}
}

// 05/29/2004 - Hypnotoad - Limits some items from not dropping
void CGame::_PenaltyItemDrop(int iClientH, int iTotal, BOOL bIsSAattacked)
{
	int i, j, iRemainItem;
	char cItemIndexList[DEF_MAXITEMS], cItemIndex;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

	// SNOOPY: Lucky effect will prevent drops,  even of a ZEM.
	if ((m_pClientList[iClientH]->m_bIsLuckyEffect > 0) && ((iDice(1, 100) <= m_pClientList[iClientH]->m_bIsLuckyEffect)))
	{
		return;
	}

	if ((m_pClientList[iClientH]->m_iAlterItemDropIndex != -1) && (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex] != NULL)) {
		// Testcode
		if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) {
			// v2.04 쩍쨘횇챈 쩔��쨘챗 쨩천횇짤쨍짰횈횆��횑쩍쨘째징 쨍횂쨈횂횁철 횊짰��횓
			// 쨈챘횄쩌��청��쨍쨌횓 쨋쨀쩐챤횁철쨈횂 쩐횈��횑횇횤��횑 ��횜쨈횢쨍챕 쨈횢쨍짜 쩐횈��횑횇횤��횑 쨋쨀쩐챤횁철횁철 쩐횎째챠 ��횑 쩐횈��횑횇횤쨍쨍 쨋쨀쩐챤횁첩쨈횢. 
			if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0) {
				m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CURLIFESPAN, m_pClientList[iClientH]->m_iAlterItemDropIndex, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan, NULL, NULL);
			}
			DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);

			m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
		}
		else {
			// v2.04 testcode

			// 쨈횢쩍횄 째횏쨩철 
			for (i = 0; i < DEF_MAXITEMS; i++)
				if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)) {
					m_pClientList[iClientH]->m_iAlterItemDropIndex = i;
					if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0) {
						m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CURLIFESPAN, m_pClientList[iClientH]->m_iAlterItemDropIndex, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan, NULL, NULL);
					}
					DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);
					m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
					break;
				}

			// 쩔��쨌첫쨔횩쨩첵! 횊챰쨩첵쩌짰��횑 쩐첩쨈횢. 
			goto PID_DROP;
		}
		return;
	}

PID_DROP:;

	for (i = 1; i <= iTotal; i++) {
		iRemainItem = 0;
		ZeroMemory(cItemIndexList, sizeof(cItemIndexList));

		for (j = 0; j < DEF_MAXITEMS; j++)
		{
			if (m_pClientList[iClientH]->m_pItemList[j] != NULL)
			{
				// Don't drop personal items
				if ((m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectType != 0)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
				{
					continue;
				}

				// Don't drop equiped activable items.
				if (((m_pClientList[iClientH]->m_pItemList[j]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
					|| (m_pClientList[iClientH]->m_pItemList[j]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY))
					&& (bIsSAattacked == FALSE))
				{
					continue;
				}
				cItemIndexList[iRemainItem] = j;
				iRemainItem++;
			}
		}

		if (iRemainItem == 0) return;
		cItemIndex = cItemIndexList[iDice(1, iRemainItem) - 1];

		DropItemHandler(iClientH, cItemIndex, -1, m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_cName);
	}
}

void CGame::GetRewardMoneyHandler(int iClientH)
{
	int iRet, iEraseReq, iWeightLeft, iRewardGoldLeft;
	DWORD* dwp;
	WORD* wp;
	char* cp, cData[100], cItemName[21];
	class CItem* pItem;
	short* sp;

	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;


	// 쨀짼��쨘 횁횩쨌짰��쨩 째챔쨩챗횉횗쨈횢. 
	iWeightLeft = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);

	if (iWeightLeft <= 0) return;
	// 횁횩쨌짰��쨩 쨔횦��쨍쨌횓 쨀짧쨈짬쨈횢. <- 횄짙��쨘 쨉쨌��쨍쨌횓 쩐횈��횑횇횤��쨩 쨩챙 째첩째짙��쨘 쨍쨋쨌횄횉횠 쨉횜쩐횩 횉횕쨔횉쨌횓.
	iWeightLeft = iWeightLeft / 2;
	if (iWeightLeft <= 0) return;

	pItem = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(pItem, cItemName);

	if ((iWeightLeft / iGetItemWeight(pItem, 1)) >= m_pClientList[iClientH]->m_iRewardGold) {
		// 횈첨쨩처짹횦��쨩 쨍챨쨉횓 쨔횧��쨩 쩌철 ��횜쨈횢. 
		pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;
		iRewardGoldLeft = 0;
	}
	else {
		pItem->m_dwCount = (iWeightLeft / iGetItemWeight(pItem, 1));
		iRewardGoldLeft = m_pClientList[iClientH]->m_iRewardGold - (iWeightLeft / iGetItemWeight(pItem, 1));
	}

	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE) {
		// 쩐횈��횑횇횤��쨩 횊쨔쨉챈횉횩쨈횢.

		// 쨀짼��쨘 횈첨쨩처짹횦 쨀쨩쩔짧 째챔쨩챗.
		m_pClientList[iClientH]->m_iRewardGold = iRewardGoldLeft;

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		// 1째쨀 횊쨔쨉챈횉횩쨈횢. <- 쩔짤짹창쩌짯 1째쨀쨋천 횆짬쩔챤횈짰쨍짝 쨍쨩횉횕쨈횂 째횒��횑 쩐횈쨈횕쨈횢
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 쩐챵��쨘 쩐횈��횑횇횤��횑쨔횉쨌횓 ��책횂첩쨉횉횁철 쩐횎쩐횘쨈횢.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		// 짹횞 쨈횢��쩍 쨀짼��쨘 횈첨쨩처짹횦��쨩 쩐횏쨌횁횁횠쨈횢.
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REWARDGOLD, NULL, NULL, NULL, NULL);

		if (iEraseReq == 1) delete pItem;

		// 쩐횈��횑횇횤 횁짚쨘쨍 ��체쩌횤 
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
			// 쨍횧쩍횄횁철쨍짝 쨘쨍쨀쩐쨋짠 쩔징쨌짱째징 쨔횩쨩첵횉횩쨈횢쨍챕 횁짝째횇횉횗쨈횢.
			DeleteClient(iClientH, TRUE, TRUE);
			break;
		}
	}
	else {
		// 쨔횧��쨩 쩌철 쩐첩쨈횂 째챈쩔챙쨈횂 쩐횈쨔짬쨌짹 횄쨀쨍짰쨍짝 횉횕횁철 쩐횎쨈횂쨈횢. 

		// Centuu : DEF_NOTIFY_CANNOTCARRYMOREITEM
		SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL, NULL, NULL);
	}
}


int CGame::_iCalcMaxLoad(int iClientH)
{
	if (m_pClientList[iClientH] == NULL) return 0;

	return (((m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) * 500) + ((m_pClientList[iClientH]->m_iLevel) * 500));
}

BOOL CGame::bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex)
{
	// ��횑 쩐횈��횑횇횤��횑 +1, +2 ��횑쨍짠��횑 쨘횢��쨘 쩐횈��횑횇횤��횑쨋처쨍챕 Attribute 횉횄쨌징짹횞쨌횓 횈짱쩌쨘횆징쨍짝 ��횑쨉쩔쩍횄횇째째챠 ��횕쨔횦 쩐횈��횑횇횤��쨍쨌횓 쨘짱횉체쩍횄횇짼쨈횢.
	if (m_pClientList[iClientH] == NULL) return FALSE;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return FALSE;

	switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
	case 4:  // Dagger +1
	case 9:  // Short Sword +1
	case 13: // Main Gauge +1
	case 16: // Gradius +1
	case 18: // Long Sword +1
	case 19: // Long Sword +2
	case 21: // Excaliber +1
	case 24: // Sabre +1
	case 26: // Scimitar +1
	case 27: // Scimitar +2
	case 29: // Falchoin +1
	case 30: // Falchion +2
	case 32: // Esterk +1
	case 33: // Esterk +2
	case 35: // Rapier +1
	case 36: // Rapier +2
	case 39: // Broad Sword +1
	case 40: // Broad Sword +2
	case 43: // Bastad Sword +1
	case 44: // Bastad Sword +2
	case 47: // Claymore +1
	case 48: // Claymore +2
	case 51: // Great Sword +1
	case 52: // Great Sword +2
	case 55: // Flameberge +1
	case 56: // Flameberge +2
	case 60: // Light Axe +1
	case 61: // Light Axe +2
	case 63: // Tomahoc +1
	case 64: // Tomohoc +2
	case 66: // Sexon Axe +1
	case 67: // Sexon Axe +2
	case 69: // Double Axe +1
	case 70: // Double Axe +2
	case 72: // War Axe +1
	case 73: // War Axe +2

	case 580: // Battle Axe +1
	case 581: // Battle Axe +2
	case 582: // Sabre +2
		return TRUE;

	}
	return FALSE;
}

void CGame::ArmorLifeDecrement(int iAttackerH, int iTargetH, char cOwnerType, int iValue)
{
	int iTemp;

	if (m_pClientList[iAttackerH] == NULL) return;

	switch (cOwnerType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[iTargetH] == NULL) return;
		break;

	case DEF_OWNERTYPE_NPC:
	default:
		return;
	}

	if (m_pClientList[iAttackerH]->m_cSide == m_pClientList[iTargetH]->m_cSide) return;

	switch (iDice(1, 13)) {
	case 1:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 2:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 3:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 4:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 5:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 6:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 7:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 8:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 9:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 10:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 11:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 12:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;

	case 13:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= 50;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, NULL, NULL);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 250) {
					ReleaseItemHandler(iTargetH, iTemp, TRUE);
					SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
				ReleaseItemHandler(iTargetH, iTemp, TRUE);
			}
		}
		break;
	}
}

char CGame::_cCheckHeroItemEquipped(int iClientH)
{
	short sHeroLeggings, sHeroHauberk, sHeroArmor, sHeroHelm;

	if (m_pClientList[iClientH] == NULL) return 0;

	sHeroHelm = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
	sHeroArmor = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
	sHeroHauberk = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
	sHeroLeggings = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];

	if ((sHeroHelm < 0) || (sHeroLeggings < 0) || (sHeroArmor < 0) || (sHeroHauberk < 0)) return 0;

	if (m_pClientList[iClientH]->m_pItemList[sHeroHelm] == NULL) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroLeggings] == NULL) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroArmor] == NULL) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroHauberk] == NULL) return 0;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 403) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 411) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 419) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 423)) return 1;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 407) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 415) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 419) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 423)) return 2;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 404) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 412) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 420) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 424)) return 1;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 408) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 416) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 420) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 424)) return 2;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 405) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 413) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 421) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 425)) return 1;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 409) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 417) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 421) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 425)) return 2;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 406) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 414) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 422) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 426)) return 1;

	if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 410) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 418) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 422) &&
		(m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 426)) return 2;

	return 0;
}

void CGame::_bDecodeDupItemIDFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iIndex = 0;
	class CStrTok* pStrTok;

	pContents = new char[dwMsgSize + 1];
	ZeroMemory(pContents, dwMsgSize + 1);
	memcpy(pContents, pData, dwMsgSize);

	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();

	while (token != NULL) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					// 스킬 번호 
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}

					if (m_pDupItemIDList[atoi(token)] != NULL) {
						// 이미 할당된 번호가 있다. 에러이다.
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Duplicate magic number.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[atoi(token)] = new class CItem;
					iIndex = atoi(token);

					cReadModeB = 2;
					break;

				case 2:
					// m_sTouchEffectType
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectType = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// m_sTouchEffectValue1
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue1 = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					// m_sTouchEffectValue2
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue2 = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// m_sTouchEffectValue3
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue3 = atoi(token);
					cReadModeB = 6;
					break;

				case 6:
					// m_wPrice
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_wPrice = (WORD)atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;

					break;
				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "DupItemID", 9) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}

		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) ERROR! DupItemID configuration file contents error!");
		return;
	}

	wsprintf(cTxt, "(!) DupItemID(Total:%d) configuration - success!", iIndex);
	PutLogList(cTxt);
}

BOOL CGame::_bCheckDupItemID(CItem* pItem)
{
	int i;

	for (i = 0; i < DEF_MAXDUPITEMID; i++)
		if (m_pDupItemIDList[i] != NULL) {
			if ((pItem->m_sTouchEffectType == m_pDupItemIDList[i]->m_sTouchEffectType) &&
				(pItem->m_sTouchEffectValue1 == m_pDupItemIDList[i]->m_sTouchEffectValue1) &&
				(pItem->m_sTouchEffectValue2 == m_pDupItemIDList[i]->m_sTouchEffectValue2) &&
				(pItem->m_sTouchEffectValue3 == m_pDupItemIDList[i]->m_sTouchEffectValue3)) {
				// 째징째횦 횁짚쨘쨍쨍짝 째쨩쩍횇횉횕째챠 쨍짰횇횕.
				pItem->m_wPrice = m_pDupItemIDList[i]->m_wPrice;
				return TRUE;
			}
		}

	return FALSE;
}

void CGame::_AdjustRareItemValue(CItem* pItem)
{
	DWORD dwSWEType, dwSWEValue;
	double dV1, dV2, dV3;

	if ((pItem->m_dwAttribute & 0x00F00000) != NULL) {
		dwSWEType = (pItem->m_dwAttribute & 0x00F00000) >> 20;
		dwSWEValue = (pItem->m_dwAttribute & 0x000F0000) >> 16;
		// 횊챰짹횒 쩐횈��횑횇횤 횊쩔째첬 횁쩐쨌첫: 
		// 0-None 1-횉횎쨩챙짹창쨈챘쨔횑횁철횄횩째징 2-횁횩쨉쨋횊쩔째첬 3-횁짚��횉��횉 
		// 5-쨔횓횄쨍��횉 6-째징쨘짯쩔챤 7-쩔쨔쨍짰횉횗 8-째짯횊짯쨉횊 9-째챠쨈챘쨔짰쨍챠��횉
		if (iDice(1, 100) < m_iRareDropRate / 100) { // Centuu : Agregado para controlar el drop rare.
			switch (dwSWEType) {
			case 0:
				break;

			case 5: // 쨔횓횄쨍��횉 
				pItem->m_cSpeed--;
				if (pItem->m_cSpeed < 0) pItem->m_cSpeed = 0;
				break;

			case 6: // 째징쨘짯쩔챤 
				dV2 = (double)pItem->m_wWeight;
				dV3 = (double)(dwSWEValue * 4);
				dV1 = (dV3 / 100.0f) * dV2;
				pItem->m_wWeight -= (int)dV1;

				if (pItem->m_wWeight < 1) pItem->m_wWeight = 1;
				break;

			case 8: // 째짯횊짯쨉횊 
			case 9: // 째챠쨈챘쨔짰쨍챠��횉 
				dV2 = (double)pItem->m_wMaxLifeSpan;
				dV3 = (double)(dwSWEValue * 7);
				dV1 = (dV3 / 100.0f) * dV2;
				pItem->m_wMaxLifeSpan += (int)dV1;
				break;
			}
		}
	}
}

// Item Logging
BOOL CGame::_bItemLog(int iAction, int iGiveH, int iRecvH, class CItem* pItem, BOOL bForceItemLog)
{
	char  cTxt[1024], cTemp1[120], cTemp2[120];
	int iItemCount;
	if (pItem == NULL) return FALSE;

	// !!횁횜��횉 횉횗짹쨔쩔징 ��청쩔챘횉횘쨋짠  New Item ��횑 쨩첵짹챈쨋짠쨈횂  iGive째징 쨀횓��횕쩌철 ��횜쨈횢.
	if (m_pClientList[iGiveH]->m_cCharName == NULL) return FALSE;

	if (iAction == DEF_ITEMLOG_DUPITEMID) {
		// 쨘쨔쨩챌쨉횊 쩐횈��횑횇횤 ��첬��책 쩔채횄쨩��횑쨈횢. 
		if (m_pClientList[iGiveH] == NULL) return FALSE;
		if (m_pClientList[iGiveH]->m_cCharName == NULL) return FALSE;
		wsprintf(G_cTxt, "(!) Delete-DupItem(%s %d %d %d %d) Owner(%s)", pItem->m_cName, pItem->m_dwCount, pItem->m_sTouchEffectValue1,
			pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3,
			m_pClientList[iGiveH]->m_cCharName);
		bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, NULL, G_cTxt);
		return TRUE;
	}

	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTemp1, sizeof(cTemp1));
	ZeroMemory(cTemp2, sizeof(cTemp2));

	switch (iAction) {

	case DEF_ITEMLOG_EXCHANGE:
		if (m_pClientList[iRecvH]->m_cCharName == NULL) return FALSE;
		wsprintf(cTxt, "(%s) PC(%s)\tExchange\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
		break;

	case DEF_ITEMLOG_GIVE:
		if (m_pClientList[iRecvH]->m_cCharName == NULL) return FALSE;
		wsprintf(cTxt, "(%s) PC(%s)\tGive\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
		break;

	case DEF_ITEMLOG_DROP:
		wsprintf(cTxt, "(%s) PC(%s)\tDrop\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_GET:
		wsprintf(cTxt, "(%s) PC(%s)\tGet\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_MAKE:
		wsprintf(cTxt, "(%s) PC(%s)\tMake\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_DEPLETE:
		wsprintf(cTxt, "(%s) PC(%s)\tDeplete\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_BUY:
		iItemCount = iRecvH;
		wsprintf(cTxt, "(%s) PC(%s)\tBuy\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, iItemCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_SELL:
		wsprintf(cTxt, "(%s) PC(%s)\tSell\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_RETRIEVE:
		wsprintf(cTxt, "(%s) PC(%s)\tRetrieve\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_DEPOSIT:
		wsprintf(cTxt, "(%s) PC(%s)\tDeposit\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_UPGRADEFAIL:
		wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Fail\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
			pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_UPGRADESUCCESS:
		wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Success\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
			pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;
	default:
		return FALSE;
	}
	bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, NULL, cTxt);
	return TRUE;
}

BOOL CGame::_bItemLog(int iAction, int iClientH, char* cName, class CItem* pItem)
{
	if (pItem == NULL) return FALSE;
	if (_bCheckGoodItem(pItem) == FALSE) return FALSE;
	if (iAction != DEF_ITEMLOG_NEWGENDROP)
	{
		if (m_pClientList[iClientH] == NULL) return FALSE;
	}
	char  cTxt[200], cTemp1[120];
	//  쨌횓짹횞 쨀짼짹채쨈횢. 
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTemp1, sizeof(cTemp1));
	if (m_pClientList[iClientH] != NULL) m_pClientList[iClientH]->m_pXSock->iGetPeerAddress(cTemp1);

	switch (iAction) {

	case DEF_ITEMLOG_NEWGENDROP:
		if (pItem == NULL) return FALSE;
		wsprintf(cTxt, "NPC(%s)\tDrop\t%s(%d %d %d %d)", cName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
		break;
	case DEF_ITEMLOG_SKILLLEARN:
	case DEF_ITEMLOG_MAGICLEARN:
		if (cName == NULL) return FALSE;
		if (m_pClientList[iClientH] == NULL) return FALSE;
		wsprintf(cTxt, "PC(%s)\tLearn\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;
	case DEF_ITEMLOG_SUMMONMONSTER:
		if (cName == NULL) return FALSE;
		if (m_pClientList[iClientH] == NULL) return FALSE;
		wsprintf(cTxt, "PC(%s)\tSummon\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;
	case DEF_ITEMLOG_POISONED:
		if (m_pClientList[iClientH] == NULL) return FALSE;
		wsprintf(cTxt, "PC(%s)\tBe Poisoned\t \t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;

	case DEF_ITEMLOG_REPAIR:
		if (cName == NULL) return FALSE;
		if (m_pClientList[iClientH] == NULL) return FALSE;
		wsprintf(cTxt, "PC(%s)\tRepair\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;

	default:
		return FALSE;
	}
	bSendMsgToLS(MSGID_GAMEITEMLOG, iClientH, NULL, cTxt);
	return TRUE;
}

BOOL CGame::_bCheckGoodItem(class CItem* pItem)
{
	if (pItem == NULL) return FALSE;

	if (pItem->m_sIDnum == 90)
	{
		if (pItem->m_dwCount > 10000) return TRUE;  //Gold쩔징 횉횗횉횠 10000쩔첩 ��횑쨩처쨍쨍 쨌횓짹횞쩔징 쨀짼짹채쨈횢.
		else return FALSE;
	}
	switch (pItem->m_sIDnum) {
		// Gold 횄횩째징 
	case 259:
	case 290:
	case 291:
	case 292:
	case 300:
	case 305:
	case 308:
	case 311:
	case 334:
	case 335:
	case 336:
	case 338:
	case 380:
	case 381:
	case 382:
	case 391:
	case 400:
	case 401:
	case 490:
	case 491:
	case 492:
	case 508:
	case 581:
	case 610:
	case 611:
	case 612:
	case 613:
	case 614:
	case 616:  // 쨉짜쨍처-쩍쩍쨌쨔��횑쩐챤
	case 618:  // 쨈횢횇짤쩔짚횉횁-쨘쨍쩔챙

	case 620:
	case 621:
	case 622:
	case 623:

	case 630:
	case 631:

	case 632:
	case 633:
	case 634:
	case 635:
	case 636:
	case 637:
	case 638:
	case 639:
	case 640:
	case 641:

	case 642:
	case 643:

	case 644:
	case 645:
	case 646:
	case 647:

	case 650:
	case 654:
	case 655:
	case 656:
	case 657:

	case 700: 	// v2.03 쨩처쩐챤 쩐횈��횑횇횤 
	case 701:
	case 702:
	case 703:
	case 704:
	case 705:
	case 706:
	case 707:
	case 708:
	case 709:
	case 710:
	case 711:
	case 712:
	case 713:
	case 714:
	case 715:

	case 720:
	case 721:
	case 722:
	case 723:

	case 724:
	case 725:
	case 726:
	case 727:
	case 728:
	case 729:
	case 730:
	case 731:
	case 732:
	case 733:

	case 734:
	case 735:

	case 736:  // 쨩천쨌횓쩔챤 쨩처쩐챤��횉 ��횣��횑쩐챨횈짰 쩌횘쨉책
	case 737:  // 쨩천쨌횓쩔챤 횊챈짹창쨩챌��횉 ��횣��횑쩐챨횈짰 쩌횘쨉책
	case 738:  // 쨩천쨌횓쩔챤 횊챈쨍쨋쨔첵쨩챌��횉쨍횇횁첨쩔첩쨉책

		return TRUE;  //횈짱쨘째횉횗 쩐횈��횑횇횤��횑짹창 쨋짬쩍횄 짹창쨌횕...

	default:
		// v2.17 2002-7-31 횁짝��횤 쩐횈��횑횇횤쨉쨉 쨌횓짹횞쩔징 쨀짼째횚 횉횗쨈횢.
		if ((pItem->m_dwAttribute & 0xF0F0F001) == NULL) return FALSE;  //횈짱쨘째횉횗 쩐횈��횑횇횤쨉횓 쩐횈쨈횕짹쨍 횈짱쩌쨘횆징쨉쨉 쩐첩쨈횢쨍챕 쨘째쨌챌..
		else if (pItem->m_sIDnum > 30) return TRUE;  //횈짱쨘째횉횗 쩐횈��횑횇횤��쨘 쩐횈쨈횕횁철쨍쨍 횈짱쩌쨘횆징째징 ��횜째챠 쨈횥째횏쨌첫째징 쩐횈쨈횕쨋처쨍챕 횁횁��쨘 쩐횈횇횤..
		else return FALSE;  //횈짱쨘째횉횗 쩐횈��횑횇횤쨉횓 쩐횈쨈횕짹쨍 횈짱쩌쨘횆징쨈횂 ��횜횁철쨍쨍 쨈횥째횏쨌첫쨋처쨍챕 쨘째쨌챌...
	}
}

void CGame::RequestSellItemListHandler(int iClientH, char* pData)
{
	int i, * ip, iAmount;
	char* cp, cIndex;
	struct {
		char cIndex;
		int  iAmount;
	} stTemp[12];

	if (m_pClientList[iClientH] == NULL) return;

	cp = (char*)(pData + 6);
	for (i = 0; i < 12; i++) {
		stTemp[i].cIndex = *cp;
		cp++;

		ip = (int*)cp;
		stTemp[i].iAmount = *ip;
		cp += 4;
	}

	// 쨀쨩쩔챘��쨩 쨈횢 ��횖쩐첬쨈횢. 쩌첩쩌짯쨈챘쨌횓 횈횊쩐횈횆징쩔챤쨈횢.
	for (i = 0; i < 12; i++) {
		cIndex = stTemp[i].cIndex;
		iAmount = stTemp[i].iAmount;

		if ((cIndex == -1) || (cIndex < 0) || (cIndex >= DEF_MAXITEMS)) return;
		if (m_pClientList[iClientH]->m_pItemList[cIndex] == NULL) return;

		// 쩔징 횉횠쨈챌횉횕쨈횂 쩐횈��횑횇횤��쨩 횈횉쨈횢.
		ReqSellItemConfirmHandler(iClientH, cIndex, iAmount, NULL);
		// ��횑 쨌챌횈쩐��쨩 쩌철횉횪횉횗 쨈횢��쩍 횇짭쨋처��횑쩐챨횈짰째징 쨩챔횁짝쨉횉쩐첬��쨩 쩌철 ��횜��쨍쨈횕 횁횜��횉!

	}
}

int CGame::iGetItemWeight(CItem* pItem, int iCount)
{
	int iWeight;

	// 쩐횈��횑횇횤��횉 쩌철쨌짰쩔징 쨉청쨍짜 쨔짬째횚쨍짝 째챔쨩챗횉횗쨈횢. Gold��횓 째챈쩔챙 쨔짬째횚쨍짝 20쨘횖��횉 1쨌횓 쨘짱째챈 
	iWeight = (pItem->m_wWeight);
	if (iCount < 0) iCount = 1;
	iWeight = iWeight * iCount;
	if (pItem->m_sIDnum == 90) iWeight = iWeight / 20;
	if (iWeight <= 0) iWeight = 1;

	return iWeight;
}

BOOL CGame::bCopyItemContents(CItem* pCopy, CItem* pOriginal)
{
	if (pOriginal == NULL) return FALSE;
	if (pCopy == NULL) return FALSE;

	pCopy->m_sIDnum = pOriginal->m_sIDnum;					// 쩐횈��횑횇횤��횉 째챠��짱 쨔첩횊짙 
	pCopy->m_cItemType = pOriginal->m_cItemType;
	pCopy->m_cEquipPos = pOriginal->m_cEquipPos;
	pCopy->m_sItemEffectType = pOriginal->m_sItemEffectType;
	pCopy->m_sItemEffectValue1 = pOriginal->m_sItemEffectValue1;
	pCopy->m_sItemEffectValue2 = pOriginal->m_sItemEffectValue2;
	pCopy->m_sItemEffectValue3 = pOriginal->m_sItemEffectValue3;
	pCopy->m_sItemEffectValue4 = pOriginal->m_sItemEffectValue4;
	pCopy->m_sItemEffectValue5 = pOriginal->m_sItemEffectValue5;
	pCopy->m_sItemEffectValue6 = pOriginal->m_sItemEffectValue6;
	pCopy->m_wMaxLifeSpan = pOriginal->m_wMaxLifeSpan;
	pCopy->m_sSpecialEffect = pOriginal->m_sSpecialEffect;

	//v1.432 쨍챠횁횩쨌체 째징째짢 쨩챌쩔챘 쩐횊횉횗쨈횢. 쨈챘쩍횇 횈짱쩌철 쨈횋쨌횂 쩌철횆징째징 쨉챕쩐챤째짙쨈횢.
	pCopy->m_sSpecialEffectValue1 = pOriginal->m_sSpecialEffectValue1;
	pCopy->m_sSpecialEffectValue2 = pOriginal->m_sSpecialEffectValue2;

	pCopy->m_sSprite = pOriginal->m_sSprite;
	pCopy->m_sSpriteFrame = pOriginal->m_sSpriteFrame;

	pCopy->m_cApprValue = pOriginal->m_cApprValue;
	pCopy->m_cSpeed = pOriginal->m_cSpeed;

	pCopy->m_wPrice = pOriginal->m_wPrice;
	pCopy->m_wWeight = pOriginal->m_wWeight;
	pCopy->m_sLevelLimit = pOriginal->m_sLevelLimit;
	pCopy->m_cGenderLimit = pOriginal->m_cGenderLimit;

	pCopy->m_sRelatedSkill = pOriginal->m_sRelatedSkill;

	pCopy->m_cCategory = pOriginal->m_cCategory;
	pCopy->m_bIsForSale = pOriginal->m_bIsForSale;
	// 

	pCopy->m_dwCount = pOriginal->m_dwCount;
	pCopy->m_sTouchEffectType = pOriginal->m_sTouchEffectType;
	pCopy->m_sTouchEffectValue1 = pOriginal->m_sTouchEffectValue1;
	pCopy->m_sTouchEffectValue2 = pOriginal->m_sTouchEffectValue2;
	pCopy->m_sTouchEffectValue3 = pOriginal->m_sTouchEffectValue3;
	pCopy->m_cItemColor = pOriginal->m_cItemColor;
	pCopy->m_sItemSpecEffectValue1 = pOriginal->m_sItemSpecEffectValue1;
	pCopy->m_sItemSpecEffectValue2 = pOriginal->m_sItemSpecEffectValue2;
	pCopy->m_sItemSpecEffectValue3 = pOriginal->m_sItemSpecEffectValue3;
	pCopy->m_wCurLifeSpan = pOriginal->m_wCurLifeSpan;
	pCopy->m_dwAttribute = pOriginal->m_dwAttribute;

	return TRUE;
}