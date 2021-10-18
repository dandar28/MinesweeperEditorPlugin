# MinesweeperEditorPlugin
Unreal Engine 4's plugin which adds a button to the editor's toolbar which allows you to open up a window running Minesweeper game.

### Game Play Info

***

##### UX and Commands

For opening the game window, you can click on the MinesweeperEditorPlugin button on the toolbar.

When clicking on MinesweeperEditorPlugin button:
- When there is no existing game window:
  - A new game window gets opened and shown.
- When a game window is already opened, a new one is not created but instead the existing one minimization is toggled:
  - If minimized, it gets restored.
  - If not minimized, it gets minimized.

The game window's UI is divided in two areas: 
- Settings area
- Main game area

***

**Settings Area**

There are 3 possible setting entries:
- Width
- Height
- Number Of Mines

There is a selectable difficulty level dropdown option that can be set to change these settings.

There are 3 default difficulty levels: 
- Beginner
  - 9x9 board
  - 10 bombs
- Intermediate
  - 16x16 board
  - 40 bombs
- Expert
  - 30x30 board
  - 99 bombs
  
There is also a Custom difficulty option in order to be able to edit the Width, Height and Number Of Mines settings.

Below all the settings there is a Play button for starting the game with the current selected settings.

***

**Main Game Area**

When shown, the main game area contains:
- Number of expected remaining mines.
- Timer from the beginning of the game being played.
- Replay button to replay the last played game (enabled only when the game has finished already).
- Game board matrix with minesweeper cells.

***

**Flow**

When the game is not started yet:
- The main game area is hidden.
- The settings are editable.

When the Play button is clicked:
- The game is started.
- The settings are not editable anymore.
- The Play button becomes a Stop button.

Interacting with a Cell:
- Mouse Left Button : Sweep.
  - If empty, reveal the cell and all the adjacents recursively until a cell with adjacent bombs is found.
  - If bomb, game over.
- Mouse Middle Button : Question Mark.
  - A question mark is placed on the cell as a reminder note. It does not affect the remaining bombs' counter.
- Mouse Right Button : Flag.
  - A flag is placed on the cell as a sign of a certain bomb. It decreases the remaining bombs' counter by 1.

When the game ends:
- An outcome text is shown over the center of the game board telling if the game is won or lost.
- The Replay button becomes enabled to allow the player to rewatch the game.

When the Stop button is clicked:
- The game is stopped.
- The settings are enabled back again.
- The Stop button becomes a Play button.

When closing the main game window, the current game session is shut down.

***

### Media

Here are some videos updated at release 1.0 :

https://user-images.githubusercontent.com/6795220/137693762-b0430ead-73bb-48bd-8233-aab0aa8f1c45.mp4


https://user-images.githubusercontent.com/6795220/137693782-449c6262-6af8-48f3-abed-88dd654797ea.mp4


https://user-images.githubusercontent.com/6795220/137693792-99d25e5e-2ca9-488e-9b45-2066734b4258.mp4


https://user-images.githubusercontent.com/6795220/137693807-1837d6ed-78ae-461b-821e-084690101e6d.mp4

***

### Implementation Dev-Notes

Development time:
- The development took 12 days, from October 6th to October 18th
- The development sessions were occasional and short, few hours a day a day.
- The stages have been initially scheduled on personal notes.
- The whole project has been tracked through github issues.

Few bugs have emerged during the development and have been fixed easily.

No difficulties came up from the development, it was an easy task and fun to implement.

***

### Known Issues

- With big Width X Height dimensions, the scaling gets bad.
  - https://user-images.githubusercontent.com/6795220/137694941-d8c47e8e-2ace-4d37-89a6-27b556bc29cb.mp4
  - A {**zooming system + 2d navigation system**} on the game board with a *minimum size of the game board widget* would solve the problem properly.

- If the Replay is playing and the user closes the window, the Replay keeps doing its stuff until the timer finishes.
  - The timer has no conditions for exiting while waiting for next actions.
  - At least, actions are properly skipped on game thread when the game session has been shut down, in order to prevent potential crashes and unwanted executions.

***

### Missing Scheduled Features (but not required)

- There are no automation tests and unit tests.

- Replay has no timeline to move through the replay execution by user interaction.
  - Replay can only go forward, it can't go backward.

- The visual design of certain texts is ugly but it would have been cool if it was more appealing.

- There is no "?" helper button for showing the commands and rules of the game when the user interacts.