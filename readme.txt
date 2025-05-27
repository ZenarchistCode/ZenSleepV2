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