#include "View.h"

#include "Widget.h"

#include <SDL.h>



View::View(SDL_Window* window, SDL_Renderer* renderer)
	: hoveredWidget( 0 ), font( 0 ), window(window), renderer(renderer)
{

}

View::~View()
{
	for ( Widget* w : widgets ) {
		delete w;
	}
}

void View::render()
{
	// Clear screen.
	SDL_SetRenderDrawColor(renderer, backgroundColor.getR(), backgroundColor.getG(), backgroundColor.getB(), 0xFF);
	SDL_RenderClear(renderer);

	// Render widgets.
	for ( Widget* w : widgets ) {
		w->render(renderer);
	}

	// Present.
	SDL_RenderPresent(renderer);
}

void View::addWidget( Widget* w )
{
	widgets.push_back( w );
	w->view = this;
	w->onAddToView(this);
}

void View::setBackgroundColor(unsigned char r, unsigned char g, unsigned char b)
{
	backgroundColor = Color(r, g, b);
}

void View::handleEvent( const SDL_Event& e )
{
	switch (e.type) {
	// Mouse down event.
	case SDL_MOUSEBUTTONDOWN:
		{
			Widget* w = this->getHoveredWidget();
			if (w) {
				w->onMouseClick();
			}
		}
		break;

	// Mouse move event.
	case SDL_MOUSEMOTION:
		{
			Widget* w = this->getHoveredWidget();

			// Mouse hover mechanic (Proceed to switch).
			if (this->hoveredWidget != w) {
				if (this->hoveredWidget) {
					this->hoveredWidget->onMouseUnhover();
				}

				this->hoveredWidget = w;

				if (this->hoveredWidget) {
					this->hoveredWidget->onMouseHover();
				}
			}
		}
		break;
	}
}

Widget* View::getHoveredWidget() const
{
	for (Widget* w : widgets) {
		if (w->isMouseHovering()) {
			return w;
		}
	}
	return 0;
}

void View::getMousePos(int* x, int* y) const
{
	int ww, wh;
	SDL_GetWindowSize(window, &ww, &wh);

	SDL_GetMouseState(x, y);
	*x *= 480;
	*y *= 900;

	*x /= ww;
	*y /= wh;
}

int View::getWidth() const
{
	int w;
	SDL_GetWindowSize(window, &w, 0);
	return w;
}

int View::getHeight() const
{
	int h;
	SDL_GetWindowSize(window, 0, &h);
	return h;
}
