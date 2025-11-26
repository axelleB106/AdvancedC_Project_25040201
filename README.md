I choose to keep my notes at the beginning of the menu file so the teacher can see the project construction steps

Menu  
--The user can choose whether to start a new factory (0 = reset) or continue with the existing one. --> Careful - if reset all data is lost   
In the menu, always create a room before doing anything else. Otherwise, the program will ask for a room ID that does not exist, causing an infinite loop: "this room is not available – please try again"    
To verify if a room is already created, select "display all rooms"

Room  

--Each room is represented as a matrix
- Columns = x = n  
- Rows = y = m  
For user input, I chose to transform x and y the way a normal user would view a floor plan: the bottom-left corner corresponds to (1,1), not (m,0) like in computer indexing.  

--Room matrix indication:  
- 0 = empty spot
- 1 = machine
- 2 = door
  
--A room’s machine file is only created when machines are added to the room. If the user chooses to reset, every room file will also be deleted.  

Machines  
--Machines cannot block the path to the door. Be careful not to call "create machine" when the room is full, otherwise an infinite loop will occur due to no more available space. Always check the room display beforehand.
