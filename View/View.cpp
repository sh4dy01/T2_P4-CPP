#include "View.h"
#include "App/App.h"
#include "App/bib.h"

#include "Widget.h"

#include <SDL.h>
#include <iostream>



View::View(SDL_Window* window, SDL_Renderer* renderer)
	: hoveredWidget( 0 ), focusedWidget( 0 ), font( 0 ), window(window), renderer(renderer)
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
		if(w->isVisible()){
			w->render(renderer);
		}
	}

	// Present.
	SDL_RenderPresent(renderer);
}

void View::update()
{
}

bool View::IsInputsNumeric(InputText** inputs, int size)
{
	for (int i = 0; i < size; i++)
	{
		const char* text = inputs[i]->getText();

		if (strlen(text) > 0) {
			for (int i = 0; i < strlen(text); i++)
			{
				if (!isdigit(text[i])) {
					inputs[i]->setText("");
					std::cout << "Non numerical char: " << text[i] << std::endl;

					return false;
				}
			}
		}
		else {
			std::cout << "Input n�" << i << " is empty" << std::endl;

			return false;
		}
	}

	return true;
}

bool View::IsInputsMoreThanZero(InputText** inputs, int size)
{
	for (int i = 0; i < size; i++) {
		const char* text = inputs[i]->getText();
		std::cout << text << std::endl;
		if (std::stoi(text) <= 0)
		{
			inputs[i]->setText("");
			std::cout << "Isn't superior to 0: " << text << std::endl;

			return false;
		}
	}
		
	return true;
}

void View::ResetInputs(InputText** inputs, int size, Color* color)
{
	Bib* bib = App::GetBibi();
	const std::string bibInfos[3] = { 
		std::to_string(App::GetBibi()->GetMaxBib()),
		std::to_string(bib->GetBibQty()), 
		std::to_string(bib->GetMinFeed()) 
	};

	for (int i = 0; i < size; i++) {
		inputs[i]->setColor(color->getR(), color->getG(), color->getB());
		inputs[i]->setText(bibInfos[i].c_str());
		inputs[i]->setPlaceholder(bibInfos[i].c_str());
	}
}

void View::addWidget( Widget* w )
{
	widgets.push_back( w );
	w->view = this;
	w->onAddToView(this);
}

void View::removeWidget(Widget* w)
{
	if (this->hoveredWidget == w) this->hoveredWidget = 0;
	if (this->focusedWidget == w) this->focusedWidget = 0;

	// Remove all occurrences of w.
	widgets.erase(std::remove(widgets.begin(), widgets.end(), w), widgets.end());

	// Delete widget and its resources.
	delete w;
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
			else {
				// Always unfocus when clicking inside the window.
				this->focusedWidget = 0;

				if (SDL_IsTextInputActive()) {
					SDL_StopTextInput();
				}
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

		// Text input.
	case SDL_KEYDOWN:
		{
			if (this->focusedWidget && e.key.keysym.sym == SDLK_BACKSPACE) {
				this->focusedWidget->onTextInput('\b');
			}
		}
		break;

	case SDL_TEXTINPUT:
		{
			if (this->focusedWidget) {
				this->focusedWidget->onTextInput(e.text.text[0]);
			}
		}
		break;
	}
}

Widget* View::getHoveredWidget() const
{
	for (auto i = widgets.rbegin(); i != widgets.rend(); ++i) {
		if ((*i)->isMouseHovering()) {
			return (*i);
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
