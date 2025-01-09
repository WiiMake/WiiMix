<p align="center">
  <a href="https://wiimix.vulcan.moe">
    <img src="https://github.com/WiiMake/WiiMix/blob/master/Data/dolphin-emu.png?raw=true" height="128">
  </a>
  <h1 align="center">WiiMix</h1>
</p>

#### WiiMix [ˈwiː.mɪks]:

> 1. To rediscover the Wii and earlier console generations by shuffling games and states for an unpredictable, exciting experience
> 2. A fork of the incredible [Dolphin Emulator](https://dolphin-emu.org) that adds new modes revolving around the concept of objectives: challenges that combine save states and [Retroachievements](https://retroachievements.org/)

⚠️ WiiMix does NOT CURRENTLY SUPPORT WII GAMES, as Retroachievements does not currently support Wii games; if you would like WiiMix to support Wii games, we recommend you check out the [Retroachievements Developer Guide](https://docs.retroachievements.org/developer-docs/getting-started-as-an-achievement-developer.html) and contribute to the Retroachievements project ⚠️

By combining objectives with extra functionality such as game swapping, WiiMix tests your ability to adapt and your knowledge of [supported games](https://wiimake.github.io/static-site/)

## Modes

- Bingo: travel across games and states to be the first to complete a bingo board of achievements
- Shuffle: swap between games with randomized time intervals in between
- Roguelike: the console has gone rogue! Can you complete a series of achievements while the console attempts to stop you?

## Contributing

The project is still in it's early stages. As such, we appreciate any help that you can provide, whether it's documenting a bug on the [Issues](https://github.com/WiiMake/WiiMix/issues) tab, [creating objectives](#creating-objectives), or [adding new features/fixing bugs in line with our contribution guidelines](https://github.com/WiiMake/WiiMix/blob/master/Contributing.md). If you are contributing or using the project, please follow the [code of conduct](https://github.com/WiiMake/WiiMix/blob/master/CODE_OF_CONDUCT.md); we follow the exact same policies as Dolphin Emulator and encourage people to be respectful and make positive contributions to our efforts to relive our favorite games and experience hidden gems through WiiMix.

### Creating Objectives

Objectives are the core behind every WiiMix. That's why we have a system in place so anyone can easily create them! There's just a couple simple steps:
1. Determine the achievement that you want to target on [Retroachievements](https://retroachievements.org/).
2. Create the save state for the achievement. The slot that you save it to doesn't matter, just make sure that you note which slot you save it to!
3. Click ```Emulation -> Send State -> Send Slot X```, where X is the slot that you saved your state to

From there, you'll see the following menu:

<p align="center">
  <img src="https://github.com/WiiMake/WiiMix/blob/master/Data/state-send-menu.png?raw=true" height="512">
</p>

To make it easy for people to filter and sort objectives, we recommend that you follow a couple guidelines when filling out this information:
- Title: copy the achievement title
- Description: copy the achievement description and append information about the state
- Achievement id: copy the achievement id (you can find it in the corresponding url, for example for https://retroachievements.org/achievement/460837, the achievement id would be 460837)
- Objective Types and Game Genres: select all that apply; these are useful for people who only want to focus on objectives of specific types or with specific genres
- Difficulty: how difficult you believe the objective would be for the average player
- Time: how much time, in minutes, you believe it would take the average player to complete the objective

PLEASE before you upload an objective, make sure the objective (same achievement, same state) DOES NOT ALREADY EXIST!
=======

## Support

Our goal is to eventually support Windows, Mac, and Linux systems; TODO finish write up on the current state of each platform before release  

## Contributing

The project is still in it's early stages. As such, we appreciate any help that you can provide, whether it's documenting a bug on the [Issues](https://github.com/WiiMake/WiiMix/issues) tab, [creating objectives](#creating-objectives), or [adding new features/fixing bugs in line with our contribution guidelines](https://github.com/WiiMake/WiiMix/blob/master/Contributing.md). If you are contributing or using the project, please follow the [code of conduct](https://github.com/WiiMake/WiiMix/blob/master/CODE_OF_CONDUCT.md); we follow the exact same policies as Dolphin Emulator and encourage people to be respectful and make positive contributions to our efforts to relive our favorite games and experience hidden gems through WiiMix.

### Creating Objectives

TODO: finish documenting this once this system is finished. Use pics.
