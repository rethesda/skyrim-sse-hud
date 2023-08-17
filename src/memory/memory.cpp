#include "memory.h"
#include <utils/PlayerDataProvider.h>
#include <utils/GeneralUtil.h>
#include <utils/NameUtil.h>



int refresh_time_data = 300;
bool startflag = false;
WeaponInfo leftWeaponInfo;
WeaponInfo rightWeaponInfo;
WeaponInfo ammoInfo;
ArmorInfo wornArmos[32];
PlayerInfo playerInfo;

void __cdecl RefreshGameInfo(void*)
{
	//auto game_hwnd = FindWindowA(NULL, "Skyrim Special Edition");
	Sleep(10000);
	//MenuOpenCloseEvent::Register();    //Register Bethesda Menu Event
	//BSTCrosshairRefEvent::Register();  //Register Bethesda Menu Event

	// 标记装备槽是否主要
	//wornArmos[1].isMainSlotAlert =
	wornArmos[2].isMainSlotAlert = wornArmos[3].isMainSlotAlert = wornArmos[5].isMainSlotAlert = wornArmos[6].isMainSlotAlert = wornArmos[7].isMainSlotAlert = true;
	wornArmos[1].isSpeacilSlotAlert = wornArmos[2].isSpeacilSlotAlert = wornArmos[3].isSpeacilSlotAlert = wornArmos[5].isSpeacilSlotAlert = wornArmos[6].isSpeacilSlotAlert = wornArmos[7].isSpeacilSlotAlert = wornArmos[9].isSpeacilSlotAlert = false;
	for (int i = 0; i <= 31; i++) {
		wornArmos[i].equipSlotName = GetEquipSlotName(i);
	}

	while (true) {
		if (refresh_time_data < 50) {
			refresh_time_data = 50;
		}
		Sleep(refresh_time_data);
		if (startflag) {
			/*	RE::UI* ui = RE::UI::GetSingleton();
			const auto menu = ui ? ui->GetMenu<RE::ContainerMenu>() : nullptr;
			const auto movie = menu ? menu->uiMovie : nullptr;
			if (movie) {

			}*/

			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
			if (player) {
				playerInfo.Angle = player->GetAngle();
				playerInfo.Position = player->GetPosition();
				playerInfo.name = player->GetName();

				//__try {
				auto playerFormEditorID = player->GetFormEditorID();
				auto playerFormID = player->GetFormID();
				auto playerGoldValue = player->GetGoldValue();
				auto playerGoldAmount = player->GetGoldAmount();
				auto playerDisplayFullName = player->GetDisplayFullName();

				auto playerLevel = player->GetLevel();
				auto playerRace = player->GetRace();
				if (playerRace) {
					//playerRaceName = player->GetRace()->GetFullName();
				}

				auto playerAttackingWeapon = player->GetAttackingWeapon();
				auto playerEquippedEntryDataLeft = player->GetEquippedEntryData(true);
				auto playerEquippedEntryDataRight = player->GetEquippedEntryData(false);
				auto playerEquippedObjectLeft = player->GetEquippedObject(true);
				auto playerEquippedObjectRight = player->GetEquippedObject(false);

				//auto id = player->GetCrimeGoldValue();
				auto factionOwner = player->GetFactionOwner();
				if (factionOwner) {
					auto factionOwnerName = factionOwner->GetFullName();
				}
				// 轻甲等级
				//lightArmor = player->GetActorValue(RE::ActorValue::kLightArmor);
				//// 重甲等级
				//heavyArmor = player->GetActorValue(RE::ActorValue::kHeavyArmor);
				//
				//player->GetLevel();

				playerInfo.kHealth = player->GetActorValue(RE::ActorValue::kHealth);
				playerInfo.kMagicka = player->GetActorValue(RE::ActorValue::kMagicka);
				playerInfo.kStamina = player->GetActorValue(RE::ActorValue::kStamina);

				playerInfo.kHealthBase = player->GetPermanentActorValue(RE::ActorValue::kHealth);
				playerInfo.kStaminaBase = player->GetPermanentActorValue(RE::ActorValue::kStamina);
				playerInfo.kMagickaBase = player->GetPermanentActorValue(RE::ActorValue::kMagicka);

				// 生命恢复速率
				auto kHealRate = player->GetActorValue(RE::ActorValue::kHealRate);
				// 战斗中生命恢复速率
				auto kCombatHealthRegenMultiply = player->GetActorValue(RE::ActorValue::kCombatHealthRegenMultiply);
				// 魔法恢复速率
				auto kMagickaRate = player->GetActorValue(RE::ActorValue::kMagickaRate);
				// 体力恢复速率
				auto KStaminaRate = player->GetActorValue(RE::ActorValue::KStaminaRate);

				playerInfo.kOneHandedModifier = player->GetActorValue(RE::ActorValue::kOneHandedModifier);
				playerInfo.kTwoHandedModifier = player->GetActorValue(RE::ActorValue::kTwoHandedModifier);
				playerInfo.kMarksmanModifier = player->GetActorValue(RE::ActorValue::kMarksmanModifier);
				playerInfo.kSmithingModifier = player->GetActorValue(RE::ActorValue::kSmithingModifier);
				playerInfo.kSmithingPowerModifier = player->GetActorValue(RE::ActorValue::kSmithingPowerModifier);
				playerInfo.kSmithingSkillAdvance = player->GetActorValue(RE::ActorValue::kSmithingSkillAdvance);
				playerInfo.kAlchemyModifier = player->GetActorValue(RE::ActorValue::kAlchemyModifier);
				playerInfo.kAlchemySkillAdvance = player->GetActorValue(RE::ActorValue::kAlchemySkillAdvance);
				playerInfo.kAlchemyPowerModifier = player->GetActorValue(RE::ActorValue::kAlchemyPowerModifier);
				playerInfo.kEnchantingModifier = player->GetActorValue(RE::ActorValue::kEnchantingModifier);
				playerInfo.kEnchantingPowerModifier = player->GetActorValue(RE::ActorValue::kEnchantingPowerModifier);
				playerInfo.kEnchantingSkillAdvance = player->GetActorValue(RE::ActorValue::kEnchantingSkillAdvance);

				// 武器伤害
				//kMeleeDamage = player->GetActorValue(RE::ActorValue::kMeleeDamage);
				// 空手伤害
				//kUnarmedDamage = player->GetActorValue(RE::ActorValue::kUnarmedDamage);

				// 弓箭伤害
				playerInfo.ArrowDamage = PlayerDataProvider::getArrowDamage(player);
				// 右手伤害
				playerInfo.DamageRight = PlayerDataProvider::getDamage(player, false);
				// 左手伤害
				playerInfo.DamageLeft = PlayerDataProvider::getDamage(player, true);

				// 显示伤害
				std::string tmp = "";
				if (playerInfo.DamageLeft.empty()) {
					if (playerInfo.DamageRight.empty()) {
						tmp = "0";
					} else {
						tmp = playerInfo.DamageRight;
					}
				} else {
					if (playerInfo.DamageRight.empty()) {
						tmp = playerInfo.DamageLeft;
					} else {
						tmp.append(playerInfo.DamageLeft);
						tmp.append(" | ");
						tmp.append(playerInfo.DamageRight);
					}
				}
				if (!playerInfo.ArrowDamage.empty()) {
					tmp.append("   - 箭:");
					tmp.append(playerInfo.ArrowDamage);
				}
				playerInfo.DamageStr = tmp;

				// 护甲
				playerInfo.kDamageResist = player->GetActorValue(RE::ActorValue::kDamageResist);
				//playerInfo.DamageResist = PlayerDataProvider::getDamageResistance(player, -1, "");

				// 暴击几率
				//kCriticalChance = player->GetActorValue(RE::ActorValue::kCriticalChance);
				// 毒抗
				playerInfo.kPoisonResist = player->GetActorValue(RE::ActorValue::kPoisonResist);
				// 火炕
				playerInfo.kResistFire = player->GetActorValue(RE::ActorValue::kResistFire);
				playerInfo.kResistShock = player->GetActorValue(RE::ActorValue::kResistShock);
				playerInfo.kResistMagic = player->GetActorValue(RE::ActorValue::kResistMagic);
				playerInfo.kResistFrost = player->GetActorValue(RE::ActorValue::kResistFrost);
				playerInfo.kResistDisease = player->GetActorValue(RE::ActorValue::kResistDisease);

				// 装备信息
				const auto inv = player->GetInventory([](RE::TESBoundObject& a_object) {
					return a_object.IsArmor();
				});

				int removeIndexs = 0;  // 最后需要移除的元素索引
				for (const auto& [item, invData] : inv) {
					const auto& [count, entry] = invData;
					if (count > 0 && entry->IsWorn()) {
						const auto armor = item->As<RE::TESObjectARMO>();
						// 插槽名称(所有)
						//std::string slotNames = "";
						int parts = (int)armor->GetSlotMask();

						removeIndexs += parts;

						//int slotIndex = 0;
						for (int i = 0; i <= 31; i++) {
							int mask = 1 << i;
							if ((parts & mask) == mask) {
								/*if (slotIndex == 0) {
									slotIndex = i;
									removeIndexs += parts;
									slotNames.append(GetEquipSlotName(i));
									slotNames.append(" ");
								}*/
								wornArmos[i].isExist = true;
								wornArmos[i].name = armor->GetName();
								wornArmos[i].formID = FormIDToString(armor->GetFormID());
								wornArmos[i].formTypeName = GetFormTypeName(armor->formType.underlying());
								wornArmos[i].goldValue = armor->GetGoldValue();
								wornArmos[i].value = armor->value;
								wornArmos[i].armorRating = armor->GetArmorRating();
								wornArmos[i].armorTypeName = GetArmorTypeName(armor->GetArmorType());
								//wornArmos[i].equipSlotName = slotNames;
								//wornArmos[i].equipSlotName = GetEquipSlotName(i);
								wornArmos[i].weight = armor->weight;
								std::string tmp = wornArmos[i].name;
								tmp.append(" - ");
								tmp.append(wornArmos[i].armorTypeName);
								tmp.append(" | ");
								tmp.append(wornArmos[i].equipSlotName);
								wornArmos[i].treeId = tmp;
							}
						}
						//if (slotIndex > 0) {
						//}
					}
				}
				// 删除未占用插槽的元素
				for (int i = 0; i <= 31; i++) {
					int mask = 1 << i;
					if ((removeIndexs & mask) != mask) {
						wornArmos[i].isExist = false;
					}
				}

				// 武器信息
				auto leftWeapon = player->GetEquippedObject(true);
				if (leftWeapon) {
					leftWeaponInfo.formType = leftWeapon->GetFormType();
					leftWeaponInfo.formTypeName = GetFormTypeName(leftWeapon->formType.underlying());
					leftWeaponInfo.name = leftWeapon->GetName();
					leftWeaponInfo.formID = FormIDToString(leftWeapon->GetFormID());
					if (leftWeapon->IsWeapon()) {
						auto item = leftWeapon->As<RE::TESObjectWEAP>();
						leftWeaponInfo.isExist = true;
						leftWeaponInfo.goldValue = item->GetGoldValue();
						leftWeaponInfo.value = item->value;
						leftWeaponInfo.weaponTypeName = GetWeaponAnimationTypeName(item->GetWeaponType());
						leftWeaponInfo.damage = item->GetAttackDamage();
						leftWeaponInfo.critDamage = item->GetCritDamage();
						leftWeaponInfo.weight = item->weight;
						std::string tmp = std::string(leftWeaponInfo.name);
						tmp.append(" - ");
						tmp.append(leftWeaponInfo.weaponTypeName);
						leftWeaponInfo.treeId = tmp;
					} else if (leftWeapon->Is(RE::FormType::Spell)) {
						auto item = leftWeapon->As<RE::MagicItem>();
						leftWeaponInfo.isExist = true;
						leftWeaponInfo.goldValue = item->GetGoldValue();
						leftWeaponInfo.castingTypeName = GetCastingTypeName(item->GetCastingType());
						leftWeaponInfo.spellTypeName = GetSpellTypeName(item->GetSpellType());
						leftWeaponInfo.cost = item->CalculateMagickaCost(player);
						leftWeaponInfo.time = item->GetChargeTime();
						auto tmp = std::string(leftWeaponInfo.name);
						tmp.append(" - ");
						tmp.append(leftWeaponInfo.castingTypeName);
						leftWeaponInfo.treeId = tmp;
					} else {
						leftWeaponInfo.isExist = false;
					}
				} else {
					leftWeaponInfo.isExist = false;
				}

				auto rightWeapon = player->GetEquippedObject(false);
				if (rightWeapon) {
					rightWeaponInfo.formType = rightWeapon->GetFormType();
					rightWeaponInfo.formTypeName = GetFormTypeName(rightWeapon->formType.underlying());
					rightWeaponInfo.name = rightWeapon->GetName();
					rightWeaponInfo.formID = FormIDToString(rightWeapon->GetFormID());
					if (rightWeapon->IsWeapon()) {
						auto item = rightWeapon->As<RE::TESObjectWEAP>();
						rightWeaponInfo.isExist = true;
						rightWeaponInfo.goldValue = item->GetGoldValue();
						rightWeaponInfo.value = item->value;
						rightWeaponInfo.weaponTypeName = GetWeaponAnimationTypeName(item->GetWeaponType());
						rightWeaponInfo.damage = item->GetAttackDamage();
						rightWeaponInfo.critDamage = item->GetCritDamage();
						rightWeaponInfo.weight = item->weight;

						if (item->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandSword ||
							item->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandAxe ||
							item->GetWeaponType() == RE::WEAPON_TYPE::kBow ||
							item->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow) {
							rightWeaponInfo.isTwoHand = true;
						} else {
							rightWeaponInfo.isTwoHand = false;
						}

						auto tmp = std::string(rightWeaponInfo.name);
						tmp.append(" - ");
						tmp.append(rightWeaponInfo.weaponTypeName);
						rightWeaponInfo.treeId = tmp;
					} else if (rightWeapon->Is(RE::FormType::Spell)) {
						auto item = rightWeapon->As<RE::MagicItem>();

						rightWeaponInfo.isExist = true;
						rightWeaponInfo.goldValue = item->GetGoldValue();
						rightWeaponInfo.castingTypeName = GetCastingTypeName(item->GetCastingType());
						rightWeaponInfo.spellTypeName = GetSpellTypeName(item->GetSpellType());
						rightWeaponInfo.cost = item->CalculateMagickaCost(player);
						rightWeaponInfo.time = item->GetChargeTime();
						rightWeaponInfo.isTwoHand = item->IsTwoHanded();

						auto tmp = std::string(rightWeaponInfo.name);
						tmp.append(" - ");
						tmp.append(rightWeaponInfo.castingTypeName);
						rightWeaponInfo.treeId = tmp;
					} else {
						rightWeaponInfo.isExist = false;
					}

				} else {
					rightWeaponInfo.isExist = false;
				}

				// 弹药
				auto ammo = player->GetCurrentAmmo();
				if (ammo) {
					const auto item = ammo->As<RE::TESAmmo>();
					ammoInfo.isExist = true;
					ammoInfo.formType = item->GetFormType();
					ammoInfo.formTypeName = GetFormTypeName(item->formType.underlying());
					ammoInfo.name = item->GetName();
					ammoInfo.formID = FormIDToString(item->GetFormID());
					ammoInfo.goldValue = item->GetGoldValue();
					ammoInfo.value = item->value;
					ammoInfo.damage = item->data.damage;
					ammoInfo.weight = item->weight;
					auto tmp = std::string(ammoInfo.name);
					/*tmp.append(" - ");
					tmp.append(rightWeaponInfo.castingTypeName);*/
					ammoInfo.treeId = tmp;
				} else {
					ammoInfo.isExist = false;
				}

				/*	} __except (SehFilter(GetExceptionCode())) {
					logger::info("catch (...)");
				}*/
			} else {
				logger::info("no player"sv);
			}
		}
	}
}