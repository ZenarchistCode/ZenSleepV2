[h1]What Is This?[/h1]

This mod adds an immersive sleeping fatigue stat to the game.

It is a total re-write of my original sleeping mod and is a far superior version, compatible with 1.28 and beyond.

Click here to watch a silly short trailer explaining the mod features: https://youtu.be/gWgUmdO1BZY

This new "sleep" stat works very similarly to the hunger and thirst stats.

Over time the player will slowly get tired, indicated by the moon icon stat. Eventually they will need to use the Lie Down emote to sleep. (Doing so requires no item in hands, a hotkey can be bound under vanilla Gestures keys).

If they ignore their basic human need to sleep for too long they eventually risk negative effects such as audible yawning giving away their position, occasional visual hallucinations, reduced immunity to sickness and even random unconsciousness if they get extremely tired.

The quality of a player's sleep is affected by whether they are sleeping at night time, near fires or other sources of warmth, inside buildings or tents, or on top of the custom included sleeping bag items which spawn in the world.

By "quality" of sleep I mean:

- There's a limit to how much their fatigue stat can be recovered based on their condition
- Some conditions make them eligible to receive immunity boosts or even health gain
- The speed they recover their fatigue stat is faster if their sleeping conditions are warm & cosy

Server admins have full control over the config of the mod, so the experience may vary from server to server and some of the features or items mentioned here might not be present if the admin doesn't want them to be.

I've spent a lot of time making this mod and followed the template of vanilla systems as closely as I possibly could based on my current modding skill level of 2+ years experience to ensure it is robust to future vanilla changes and also increases the chances of it being compatible with other mods.

It should be compatible with the vast majority of mods on the workshop, but might won't work well with other complex mods which affect player stats (like the Syberia Project for example, who have their own sleeping mod anyway).

A lot of thought has gone into the mechanics and I spent several weeks building the mod and bugtesting, although as a solo dev I'm sure bugs are still present and I'll do my best to fix any which are reported. 

Here are some examples of how the mod works and why it's immersive (most of these features are configurable):

- This mod gives players a reason to chill from time to time, make a campfire, and enjoy the ambience
- Trying to sleep while cold, wet, sick, hungry, thirsty, or during the day will limit their max rest
- The player's activity level affects the speed which they drain their fatigue stat at over time
- The player has about 5 hours of full rest time as a freshy (can be configured to be more/less)
- During the night fatigue drains 50% faster when active, but also recovers 50% faster when sleeping
- Getting injured will cause the player to be unable to sleep for some time due to adrenaline
- Drinking sugary drinks like soda cans will increase their fatigue stat (fully configurable)
- Custom items (anesthetic and amphetamine injectors) boost/reduce the fatigue stat significantly
- Sleeping in a sleeping bag or tent will maintain a small heat buff to avoid illness in the cold
- Sleeping inside a building and ideally with a heat source nearby will speed up resting
- Sleeping bags can be carried on the backpack tool slot and can be repaired if damaged
- When the player yawns sometimes their screen will produce a chroma hallucination effect
- The player makes sounds while sleeping which can be heard by attentive players nearby
- The mod was made to be as efficient as possible with server FPS and netcode
- VPPAdminTools compatibility is included for debugging and admin configuration

[h1]Installation Instructions:[/h1]

Install this mod like any other mod (there are many guides out there) - copy it into your server folder and add it to your server mods list. It must be run on both server and client. Make sure to copy the .bikey into your server keys.

Also don't forget to copy over the types.xml from inside the "extras" folder into your server types if you want to add the custom items to your loot table (optional). You can either import my types.xml separately (here's a guide: https://www.youtube.com/watch?v=PAAEhdiQ_vE) - OR just carefully merge my types with the vanilla types ensuring you don't break the XML structure in the process.

The mod will automatically generate a default server config file in your server's profiles folder (under "profiles/Zenarchist/ZenSleepConfig.json"). If you're not experienced with JSON editing make sure to use this website to double-check the validity of your changes or you run the risk of the server reverting all of your changes to the default values: https://jsonlint.com/

There will be a YouTube video on my channel explaining how the mod works: https://youtu.be/7ckS1W7wAq8

And here is a short wiki guide explaining the config etc: https://github.com/ZenarchistCode/ZenModPack/wiki/Zen's-Sleeping-Mod

You DO need to wipe your server to add or remove this mod because otherwise your player profiles will become corrupted due to the new sleep stat.

This mod requires Community Framework to function (https://steamcommunity.com/workshop/filedetails/?id=1559212036)

There is an optional persistence-related PBO file in the "ZenSleep/extras/defines" mod folder (for advanced server admins/modders only) - a readme file is included in that folder to explain what it does.

[h1]Sleeping in vehicles:[/h1]

Press Z while fully seated to begin sleeping. Rebind "Sleep / wake up in vehicle" under Controls > ZEN if desired. Passengers may sleep while travelling. Drivers may sleep while the vehicle is stopped or moving at no more than 1 km/h, even if its engine is running. Moving above 1 km/h prevents starting sleep and wakes an already sleeping driver. This small tolerance ignores parked-car physics jitter. The eye-closing effect begins immediately when the sleep action starts, while fatigue recovery still waits for the normal five-second delay. Waking fades the eyes open as with normal sleep. Configured restrictions, sounds and visual effects still apply; no lie-down emote is played and the seat does not grant bed bonuses.

Press the sleep key again or another keyboard key to wake, except for the inventory open/close binding and inventory controls while inventory is open. Mouse clicks and movement do not wake you. Opening menus, showing the cursor or changing window focus does not automatically cancel sleep. Damage (including health, blood or shock lost in a crash), unconsciousness, death, leaving the vehicle or changing seats cancels vehicle sleep. Waking from a crash never removes normal unconsciousness.

The setting in profiles/Zenarchist/ZenSleepConfig.json is inside GeneralSleepConfig:

"GeneralSleepConfig": {
    "AllowVehicleSleep": true,
    ...
}

Set it to false to disable vehicle sleep, then restart the server and reconnect clients. It is enabled by default, including when upgrading an older config that omits the setting. The server enforces and explicitly synchronizes this GeneralSleepConfig option to clients. The earlier top-level AllowVehicleSleep value is migrated automatically, preserving false. The new GeneralSleepConfig value takes priority if both exist. With TerjeMedicine installed this feature follows the existing OverrideTerje selector, so ZenSleep must control sleeping for it to be available.

The rebindable UAZenSleepVehicle input is registered by ZenSleep's CfgMods inputs declaration. It can coexist with ZenModPack's own input XML. Both server and client must load the updated scripts. The sleep action is ActionZenSleepInVehicle with its own ZenSleepVehicleActionInput; rejection messages use the same server checks and DisplaySleepMessages option as normal sleep.

[h1]Optional Terje compatibility:[/h1]

Load ZenSleep after Terje's mods on both server and client. Terje is optional; ZenSleep still works with only its normal dependencies.

In profiles/Zenarchist/ZenSleepConfig.json, set the TOP-LEVEL setting:

"OverrideTerje": true

Restart the server and reconnect clients after changing the selector. The server synchronizes this setting with ZenSleep's client effects; clients do not choose their own sleep system.

- Without TerjeMedicine: ZenSleep works normally and ignores OverrideTerje.
- With TerjeMedicine and OverrideTerje=true: ZenSleep controls sleep, its moon HUD, sounds, closed eyes, blur, hallucinations, audio attenuation, beds and environmental restrictions. Terje's native sleep loop and sleep-only presentation are suppressed.
- With TerjeMedicine and OverrideTerje=false (or the setting omitted): Terje controls sleeping. ZenSleep's fatigue loop, sleep effects and fatigue debuffs are inactive. The default is false so installing this update does not silently take over Terje.

Leave Medicine.SleepingEnabled=true in TerjeSettings/Medicine.cfg if you want to swap systems using only OverrideTerje. The ZenSleep override ALSO works when Medicine.SleepingEnabled=false: it keeps Terje's sleep-stat/medicine APIs active while replacing its sleep loop. If both OverrideTerje and Medicine.SleepingEnabled are false, neither sleep system runs. Do not unload TerjeMedicine to select ZenSleep; the medical APIs and items are still needed.

The shared rest value uses Terje's persisted sleep record while the override is enabled. ZenSleep's 0-5000 scale and Terje's 0-14000 scale represent the same percentage. Terje medicines, energy drinks, sleep-damaging items/attacks, admin stat setters and start-screen presets therefore affect ZenSleep immediately. Existing Terje rest and medicine timers are retained. Zen's serialized fatigue slot is kept and saved as a matching cache; its format is unchanged.

[b]Configuration priority when OverrideTerje=true:[/b]

- ZenSleepConfig.json wins wherever both mods control the same sleep behaviour, including explicit zero/false values. Terje supplies additional mechanics without a direct Zen equivalent. This priority only applies with the override enabled; standalone ZenSleep and Terje with the override off retain their existing behaviour.
- Zen's SecondsAsleepUntilFullRestApprox recovery curve, GlobalGainMultiplier and all night/bed/shelter/heat bonuses control sleep recovery. Zen's GlobalDrainMultiplier, movement, weight, heavy-item and night settings control awake drain. These replace SleepingIncCommon, SleepingIncComfort and SleepingDecPerSecCommon; no second Terje baseline drain is subtracted while sleeping.
- The moon and its arrows use Zen's fatigue scale and configured gain/drain calculation. With the default Zen drain settings, standing still produces one down arrow, including at night with DrainNightMultiplier=1.5. Medicine and activity can still legitimately change the tendency.
- The current Fast Sleep perk (immunity/fsleep) multiplies Zen recovery by 1 + the player's live perk value, including customized perk values and eligibility rules. TerjeSkills is not required; without it the multiplier is 1.
- Zen's MaxRestGain_Illness remains active for its normal sickness check. Terje's SleepingBlockWithWounds adds its separate serious illness/injury restriction through HasTerjeSicknesOrInjures. This includes sepsis levels 2 and 3, severe pain, wounds and Terje's other critical conditions. A blocker prevents starting sleep and invalidates existing sleep immediately. Beds do not bypass this check.
- Zen's MaxFatiguePercentToAllowSleeping controls starting sleep; it does not stop an already sleeping player from recovering above that percentage. Zen's day/night, temperature, wetness and food/water caps control waking. These take priority over SleepingBlockOnFull and SleepingAwakeOnFull.
- Zen's PercentGainedUponUncon controls its existing rest award on waking from unconsciousness at low fatigue. It replaces SleepingIncUnconscin; there is no additional continuous Terje unconscious recovery. Zen's unconsciousness chances, intervals and driving/swimming restrictions control sleep-induced blackouts; Terje's unconditional sleep shock threshold is suppressed.
- In a dry bed, Zen's BedHealthGainPerSec, gain modifiers, BedHealthMaxGain and MinSecsAsleepToTriggerHealthRecovery control healing, applied once through Terje's health API. Zero Zen bed healing stays zero. Outside a dry bed, SleepingHealthInc still applies because Zen has no non-bed healing setting. SleepingBloodInc and SleepingMindInc remain active while genuinely asleep because Zen has no corresponding settings.
- Zen's configured bed immunity threshold grants Terje's vitamin-equivalent immunity gain for at least the vanilla vitamin lifetime, retaining stronger or longer existing medicine. FatigueLevelToNegativeImpactImmunity controls Zen's vanilla immunity adjustment; Terje's separate internal immunity and skills remain intact.
- Zen's HealthLossPerSecRedFlashing replaces SleepingHealthDec, including zero. SleepingBloodDec and SleepingMindDec retain their Terje rates and critical threshold. Godmode prevents this exhaustion damage. Zen's CharacterMovementSpeedRed/RedFlashing control the sleep limp; Terje's other injury/disease movement effects remain active. Zen's stamina settings remain active.
- For items matched by Zen's ModifierActivatedPercent, ModifierDeactivatedPercent or ModifierPausesSeconds (including inherited class matches and explicit zero entries), Zen's configured sleep effects take priority. Only that item's competing Terje sleep dose is suppressed; overdose, skill processing and other medical effects remain. Items without a matching Zen entry use Terje's dose, duration and stacking rules. Already-active Terje medicine timers are retained and consume their actual remaining duration, including partial ticks. Modifier-name entries still follow their normal modifier activation/deactivation events.
- Terje sleeping bags retain their use/pack actions and are recognized as Zen beds when deployed and usable. Medicine.EnableSleepingbagAction=false also disables Zen's duplicate sleep action on Terje bags, since Zen has no corresponding action toggle. Zen bags and configured bed objects retain Zen's bed bonuses.
- Zen's snore chance/timing settings take priority over SleepingUseSnore. Zen's moon, sounds, closed eyes, blur, hallucinations and audio attenuation replace Terje's sleep presentation; other medical effects and HUD indicators remain active.

Compatibility was audited against TerjeMods upstream commit 08208ffe75783207281dbc40ba4f4d6970cb8cb3 (https://github.com/TerjeBruoygard/TerjeMods). Future Terje sleep API changes may require updating this adapter. Custom code edits inside a private Terje sleep modifier are replaced when ZenSleep takes over; express the desired bed/environment behaviour in ZenSleep's JSON.

[h1]Credit:[/h1]

Obviously the original sleeping mod included in Medical Attention is a huge inspiration for this mod: https://steamcommunity.com/sharedfiles/filedetails/?id=2293429999

I learned a lot about the technique for adding custom stats to the game from the Syberia Project authors: https://steamcommunity.com/workshop/filedetails/?id=2569522069

I've learned a ton from DayZ modding communities, particularly the Enfusion Modders Discord (https://discord.com/invite/enfusion-modders-452035973786632194) and studying the work of the Expansion Project team (https://steamcommunity.com/workshop/filedetails/?id=2572331007).

My DayZ modding buddy SpaceCat (https://steamcommunity.com/id/spacecatboy/myworkshopfiles/?appid=221100) has taught me some important debug tricks and best practices which has no doubt led to improved stability/crash-proofness for this mod and my other recent mods.

[h1]Repack & Source Code:[/h1]

You can repack this mod if you like, and do anything else you want with it for that matter. Just keep in mind my future updates won't be applied so make sure to check back for new versions if you notice any bugs. The source code is on my GitHub at www.zenarchist.io

[h1]Buy Me A Coffee:[/h1]

All my mods are free and open source, but it takes an enormous amount of time to put some of these mods together. If I've helped you out, please consider helping me buy my next coffee! I don't expect it, but I very much appreciate it.

https://buymeacoffee.com/zenarchist

Enjoy!
