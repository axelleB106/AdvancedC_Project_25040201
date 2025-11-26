# AdvancedC_Project_25040201

Menu 
I choose to let my notes at the beginning of menu for the teacher to see the project construstion steps 
User can choose wether to start a new factory (0 = reset) or continue on the old one - careful if reset - no way of recuperating the old data 
- In the menu - choose to create a room before anything - else the programm will ask you a room and there will be no match - just an infinite question - "this room is not available - please try again"
- to verify if a room is already created or not - choose "display all room "

Room
-- Each room is simbolized as a matrix 
Col = x = n
Row = y = m 
-- for user input - I choose to transform x and y like a normal would think as they see a floor plan - which mean bottom left == (1:1) and not (m,0) as the computer thinks 
  - O == empty spot
  - 1 == Machine
  - 2 == Door
  - room machine file is only created when machine are input in room - if user choose to reset every room file wiil also be deleted 

Machines
-- Machine cannot block paths to the door - so be extra careful to not call create machine when the room is full - else infinite loop of no more space - check display room before 
