This fork includes new content, mechanics, and AI updates by @samrocketman.  For a full list of changes read on.

# Download

Download for your platform.

| Platform    | Download links |
| ----------- | -------------- |
| Linux       | <ul><li>[endless-sky-x86_64-continuous-with-plugins.AppImage][linux.appimage]</li><li>[endless-sky-x86_64-continuous-with-plugins.tar.gz][linux.tgz]</li></ul> |
| Mac OS      | <ul><li>[EndlessSky-macOS-continuous-with-plugins.zip][mac.zip]</li></ul> |
| Windows     | <ul><li>[EndlessSky-win64-continuous-with-plugins.zip][windows.zip]</li></ul> |
| Source Code | <ul><li>[continuous-with-plugins.zip][source.zip]</li><li>[continuous-with-plugins.tar.gz][source.tgz]</li></ul> |

[linux.appimage]: https://github.com/samrocketman/endless-sky/releases/download/continuous-with-plugins/endless-sky-x86_64-continuous-with-plugins.AppImage
[linux.tgz]: https://github.com/samrocketman/endless-sky/releases/download/continuous-with-plugins/endless-sky-x86_64-continuous-with-plugins.tar.gz
[mac.zip]: https://github.com/samrocketman/endless-sky/releases/download/continuous-with-plugins/EndlessSky-macOS-continuous-with-plugins.zip
[windows.zip]: https://github.com/samrocketman/endless-sky/releases/download/continuous-with-plugins/EndlessSky-win64-continuous-with-plugins.zip
[source.zip]: https://github.com/samrocketman/endless-sky/archive/refs/tags/continuous-with-plugins.zip
[source.tgz]: https://github.com/samrocketman/endless-sky/archive/refs/tags/continuous-with-plugins.tar.gz

# Save Game Compatability

> **WARNING:** May have issues.

This is may have issues if you already have these plugins.  This may also cause
issues if you try to move your save game back to continuous because it will
include defunct missions provided by plugins.

# Plugins

> **WARNING:** If you have these plugins instaled, remove them as they would be
> duplicate and conflict with the global plugins.

The following plugins are included.

| Icon | Name | Author | License | Description |
| --- | --- | --- | --- | --- |
| ![icon][plugin-hdpi-icon] | [High DPI Continuous][plugin-hdpi] | Michael Zahniser | [CC-BY-SA-4.0][license-cc-by-sa-4.0] | High-DPI graphics for Endless Sky. |
| ![icon][plugin-lampyrid-icon] | [Lampyrid Start][plugin-lampyrid] | Zitchas | [GPL-3.0-or-later][license-gpl3] | Enables an alternate start wherein the player starts with a modified Lampyrid. |
| ![icon][plugin-lost-icon] | [Lost in Midnight][plugin-lost] | Michael Arsollon | [GPL-3.0-or-later][license-gpl3] | Lost in Midnight is a mission focused plugin for the Endless Sky game. Features new missions, systems, outfits, ships, and variants. |
| ![icon][plugin-mf-icon] | [Mega Freight][plugin-mf] | 1010todd | [GPL-3.0-or-later][license-gpl3] | Adds several mostly civilian ships for you and npc merchants (and pirates) to use for more variety in human space. |
| ![icon][plugin-scrap-icon] | [Midnight Scrapyard][plugin-scrap] | Michael Arsollon | [GPL-3.0-or-later][license-gpl3] | A plugin suggesting an epilogue for the Kor Sestor region of Endless Sky. Includes some new ships, outfits, and missions. |
| ![icon][plugin-portraits-icon] | [Portraits for News][plugin-portraits] | Anarchist2 | [GPL-3.0-or-later][license-gpl3] | This plugin re-activates the portrait system for news in ES. |

<!-- Removed plugins
| ![icon][plugin-cmi-icon] | [Core Mining, Inc.][plugin-cmi] | DJF113 | [GPL-3.0-or-later][license-gpl3] | Adds another storyline in the early game, following a business getting started in Syndicate space. |

[plugin-cmi-icon]: https://github.com/DJF113/Core-Mining-Inc/raw/v0.1.8/icon.png
[plugin-cmi]: https://github.com/DJF113/Core-Mining-Inc
-->

[license-cc-by-sa-4.0]: https://spdx.org/licenses/CC-BY-SA-4.0.html
[license-gpl3]: https://spdx.org/licenses/GPL-3.0-or-later.html
[plugin-hdpi-icon]: https://github.com/endless-sky/endless-sky-high-dpi/raw/60dc4d5f895aec6cd7e50fd22f83f0e62aa62128/icon.png
[plugin-hdpi]: https://github.com/endless-sky/endless-sky-high-dpi
[plugin-lampyrid-icon]: https://raw.githubusercontent.com/Zitchas/ES_Lampyrid_Start/v1.6/icon.png
[plugin-lampyrid]: https://github.com/Zitchas/ES_Lampyrid_Start
[plugin-lost-icon]: https://raw.githubusercontent.com/MidnightPlugins/Lost-in-Midnight/0.9.14.16.7/icon.png
[plugin-lost]: https://github.com/MidnightPlugins/Lost-in-Midnight
[plugin-mf-icon]: https://raw.githubusercontent.com/1010todd/Mega-Freight/main/icon.png
[plugin-mf]: https://github.com/1010todd/Mega-Freight
[plugin-portraits-icon]: https://github.com/Anarchist2/ES-news-portraits/raw/v1.0/icon.png
[plugin-portraits]: https://github.com/Anarchist2/ES-news-portraits
[plugin-scrap-icon]: https://raw.githubusercontent.com/MidnightPlugins/Midnight-Scrapyard/0.9.14.7.18/icon.png
[plugin-scrap]: https://github.com/MidnightPlugins/Midnight-Scrapyard

# New Features in this fork

### New Mechanics

[Fleet mining: focused fire on asteroids](https://github.com/endless-sky/endless-sky/pull/6669)

<details><summary>See feature details for fleet mining (click to expand)</summary>

---

- New shortcut `V` with a dual purpose
  - If you have asteroid scanning outfits, it will select the nearest asteroid.
  - `V` shortcut will toggle Harvest mode which means your fleet will collect harvested asteroids.  Your feel will also retrieve abandoned cargo from destroyed ships in harvest mode.
- `F` shortcut (focus fire) works on asteroids, your fleet will attack asteroids to mine them.  Your player ship must have asteroid scanning to select asteroids.
- A new preference for fighters transferring cargo to carried ships.
- A new preference for targeting asteroids based on highest value or proximity.

</details>

---

[Low powered fighters and non-combat fighter AI enhancements](https://github.com/endless-sky/endless-sky/pull/6726)

<details><summary>See feature details for low powered fighters (click to expand)</summary>

---

- Player ship, escorts, fighters, and drones can be powered only by batteries.  (No power gen)
- Carrier Tanker Refueling AI
  - When you have a carrier with lots of fuel and fighters or drones with fuel pods they can be used to refuel your escort fleet.
  - Your fleet can focus on combat while the carrier tanker can refuel them.
  - Smart refueling behavior: All ships get 1 jump first, then all escorts get 100% refueled, then all mission NPC escorts get 100% refueled.  Finally, when all escorts are refueled the fighters or drones help to refuel the parent carrier ship back to full for the next refueling run.
- Ship recovery AI updates
  - Ships out of battery will become disabled and can call for help for a recharge.  This includes escorts and the player flagship.
  - Battery powered fighters and drones will automatically return to carriers to recharge.  This includes low powered fighters which have battery and small amounts of energy generation.
  - Battery powered fighters sharing energy with other ships during recovery operations reserve enough energy to be able to return to parent carriers.  This enables battery powered fighters and drones to be effective when aiding in disabled ship recovery.
  - Fighters and drones can recover other ships and drones (including battery powered fighters and drones).  This is useful when mining in the ember wastes with battery powered fighters and drones.
  - Carriers will recover their own carried ships which were disabled during battle.
  - All ships (including carriers) will recover disabled fighters and drones which are not carried by them.  This enables fighters associated with destroyed carriers to find a new home in a new carrier with empty bays.
- "No suicide pact" for defenseless fighters AI updates
  - Fighters will refuse to launch if they have no weapons and there are enemies in the system.
  - Fighters will retreat and re-dock with carrier if they have no weapons and enemies enter the system.
  - Minimum 10 second flight time.  Fighters will refuse to deploy if they do not have sufficient energy for 10 seconds of flight time overall.  This includes worst case scenario of using weapons the entire time.
    - Vulnerable fighters are less vulnerable in battle (like boxwings) because they stay docked.
    - Fighters will only deploy if their outfits allow them sufficient flight time in battle.
- Anti-Missile Defense AI updates
  - Fighters equipped with anti-missile and no weapons will still deploy since they're not considered completely defenseless.  Their purpose is to defend the parent carrier.
  - Fighters equipped with anti-missile and no weapons should not move when player issues orders for fleet to attack enemy.  They should keep station by their parent carrier to defend the carrier from missile attacks.

</details>

---

[Bugfix: abandoned fighters quickly find new carriers](https://github.com/endless-sky/endless-sky/pull/6866)

<details><summary>See feature details for fighters finding new carriers (click to expand)</summary>

---

- If a fighter loses a parent carrier or a fighter is captured, then it will quickly pathfind to board a new valid carrier parent.

</details>

---

### New Content

Finally, new content is added by the above pull requests as well as [Bays as outfits engine support](https://github.com/endless-sky/endless-sky/pull/6792) and [fighter and drone bay outfits](https://github.com/endless-sky/endless-sky/pull/6793) pull requests.

- New tutorials:
  - Initial game tutorial is updated to introduce the player to fleet mining and southern mining drones.
  - Tutorials for fleet mining.
- New ship: a cheap battery powered mining drone is available in Southern Shipyards (less than 70k credits to purchase).
- New outfits:
  - Drone bays can be installed on gun hardpoints.  Enables any ship with guns to carry additional drones.
  - Fighter bays can be installed on turret hardpoints.  Enables any ship with turrets to carry additional fighters.
- Warning dialogs have been updated for battery powered and low powered fighters and drones.  You'll get a warning if you misconfigure your ships.
- New Campaign: [Unfettered Campaign Act 1 : preparations for the war][unfettered].

[unfettered]: https://github.com/endless-sky/endless-sky/pull/6416

# Other changes

- Includes [bugfix for SHIFT+J fleet jumping](https://github.com/endless-sky/endless-sky/pull/6973) where player fleet does not wait for non-mission escort NPCs when fleet jumping.
- [Enhancements to mining made by Zoura](https://github.com/samrocketman/endless-sky/tree/zoura-mining-jobs) such as jobs and thicker asteroids which take longer to destroy but yield more resources.


# Build Information

Triggered on ${BUILD_DATE} UTC by commit ${BUILD_COMMIT} (@${BUILD_USER})

This is an automated build of the latest source. It may be unstable or even crash, corrupt your save or eat your kitten. Use with caution!

${BUILD_URL}
