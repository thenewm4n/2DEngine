
# To Implement

1. NewShape constructor - using relevant parameters.
2. Font case.

- Read parameters from config.txt.
	- Will always be Window and Font line
	- May not be any shape lines
	- Window W H; Font F S R G B; Circle N X Y SX SY R G B R; Rectangle N X Y SX SY R G B W H

- New Shape class - what isn't in sf::Shape? -> velocities, name, drawn/not...
	- sf::Shape, draw/not, X & Y velocities, text, array for the imgui input box, width & height (if rectangle), radius & no. segments (if circle)
	- Upon instantiation: set position of shape according to file, set position of sf::Text (using text.getLocalBounds())

- Reversing of x velocity if hit left or right, reversing of y velocity if hit top or bottom.
	- shape.getLocalBounds().top/.left/.width/.height
	- shape.getGlobalBounds() (?)
	- for edge of shape, if edge exceeds corresponding edge of window, shape.correspondingVelocity *= -1


In ImGui:
- Drop box to select a shape
- Toggle if shape drawn
	- for shape in shapes, if bool draw == true, window.draw(shape)
- Change scale (from 0-4)
- X & Y velocity (from -8 to 8)	
- Change color
- Change text