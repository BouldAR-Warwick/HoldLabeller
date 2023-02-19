#include <iostream>
#include <fstream>
#include <sstream>

#include <sfml/Graphics.hpp>
#include "json.hpp"

// 1 = Jug
// 2 = Crimp
// 3 = Sloper
// 4 = Pinch
// 5 = Pocket
// 6 = Incut
// 7 = Edge
// 8 = Other
// 9 = Other
// 0 = Foothold

// Left = prev
// Right = next

using json = nlohmann::json;

inline void print(std::string id, std::string other) 
{
	std::cout << "set " << id << " to " << other << "\n";
}

void loadhold(const json& data, int* face, int* hold, sf::Texture& texture, sf::Sprite& sprite, sf::VertexArray& va, sf::View& view) {
	if (*hold < 0) {
		if (*face > 0) 
		{
			--*face;
			texture.loadFromFile("data/imgs/" + data["holds"][*face]["face"].get<std::string>() + ".jpg");
			*hold = data["holds"][*face]["holds"].size() - 1;
		}
		else
		{
			*hold = 0;
			return;
		}
	}
	if (data["holds"][*face]["holds"].size() <= *hold)
	{
		++*face;
		texture.loadFromFile("data/imgs/" + data["holds"][*face]["face"].get<std::string>() + ".jpg");
		*hold = 0;
	}

	va.clear();
	std::stringstream ss(data["holds"][*face]["holds"][*hold]["polygonStr"].get<std::string>());
	for (std::string token; ss >> token;)
	{
		std::istringstream iss(token);
		std::string t;
		float v[2] = { 0.f, 0.f };
		int i = 0;
		while (std::getline(iss, t, ',')) {
			v[i++] = std::stof(t);
		}
		va.append(sf::Vertex{ {v[0], v[1]}, sf::Color::Red });
	}
	va.append(va[0]);

	std::stringstream ss2(data["holds"][*face]["holds"][*hold]["centroidStr"].get<std::string>());
	sf::Vector2f centre;
	ss2 >> centre.x >> centre.y;

	view.setCenter(centre);
}

void loadclimb(const json& data, int face, int* routeId, std::vector<sf::VertexArray>& route)
{
	route.clear();

	if (*routeId < 0)
		*routeId = 0;

	int size = data["climbs"][face]["climbs"].size();

	if (data["climbs"][face]["climbs"].size() <= *routeId)
		*routeId = data["climbs"][face]["climbs"].size() - 1;

	route.clear();

	if (*routeId < 0)
		return;

	std::cout << "CLIMB!!\n";

	for (auto& hold : data["climbs"][face]["climbs"][*routeId]["holds"])
	{
		sf::VertexArray va{ sf::PrimitiveType::TriangleFan };
		int id = std::stoi(hold.get<std::string>());

		for (auto& h : data["holds"][face]["holds"])
		{
			if (h["id"].get<int>() == id) 
			{
				std::stringstream ss(h["polygonStr"].get<std::string>());
				for (std::string token; ss >> token;)
				{
					std::istringstream iss(token);
					std::string t;
					float v[2] = { 0.f, 0.f };
					int i = 0;
					while (std::getline(iss, t, ',')) {
						v[i++] = std::stof(t);
					}
					va.append(sf::Vertex{ {v[0], v[1]}, sf::Color{255, 255, 255, 200} });
				}
				va.append(va[0]);
				route.push_back(va);
				break;
			}
		}
	}
}

int main()
{
	sf::RenderWindow window{ sf::VideoMode{1728, 972}, "Hold classifying helper" };
	sf::View view = window.getDefaultView();

	std::ifstream datafile{"data/data.json"};
	json data = json::parse(datafile);
	datafile.close();

	json out;
	std::ifstream outfile{ "classification.json" };
	if (outfile.is_open())
	{
		out = json::parse(outfile);
		outfile.close();
	}

	int face = 0;
	int hold = 0;

	sf::Texture currentTex;
	sf::Sprite sprite;
	currentTex.loadFromFile("data/imgs/" + data["holds"][face]["face"].get<std::string>() + ".jpg");
	sprite.setTexture(currentTex);
	sf::VertexArray va{ sf::PrimitiveType::LineStrip };

	loadhold(data, &face, &hold, currentTex, sprite, va, view);

	bool displayRoute = false;
	int routeId = 0;
	std::vector<sf::VertexArray> route;
	loadclimb(data, face, &routeId, route);

	sf::Vector2i lastPos;
	int pressed = 0;
	float zoom = 1;

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if (e.type == sf::Event::Closed)
				window.close();
			else if (e.type == sf::Event::MouseWheelMoved) {
				view.zoom(e.mouseWheel.delta > 0 ? 0.5f : 2.f);
				zoom *= e.mouseWheel.delta > 0 ? 0.5f : 2.f;
			}
			else if (e.type == sf::Event::MouseButtonPressed) {
				lastPos = sf::Mouse::getPosition(window);
				pressed++;
			}
			else if (e.type == sf::Event::MouseButtonReleased)
				pressed--;
			else if (e.type == sf::Event::KeyPressed) {
				bool next = false;
				int idint = data["holds"][face]["holds"][hold]["id"].get<int>();
				std::string id = std::to_string(idint);
				if (e.key.code == sf::Keyboard::Num1 || e.key.code == sf::Keyboard::Numpad1)
				{
					out[id] = "Jug";
					print(id, "Jug");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num2 || e.key.code == sf::Keyboard::Numpad2)
				{
					out[id] = "Crimp";
					print(id, "Crimp");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num3 || e.key.code == sf::Keyboard::Numpad3)
				{
					out[id] = "Sloper";
					print(id, "Sloper");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num4 || e.key.code == sf::Keyboard::Numpad4)
				{
					out[id] = "Pinch";
					print(id, "Pinch");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num5 || e.key.code == sf::Keyboard::Numpad5)
				{
					out[id] = "Pocket";
					print(id, "Pocket");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num6 || e.key.code == sf::Keyboard::Numpad6)
				{
					out[id] = "Incut";
					print(id, "Incut");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num7 || e.key.code == sf::Keyboard::Numpad7)
				{
					out[id] = "Edge";
					print(id, "Edge");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num8 || e.key.code == sf::Keyboard::Numpad8)
				{
					out[id] = "Other-8";
					print(id, "Other-8");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num9 || e.key.code == sf::Keyboard::Numpad9)
				{
					out[id] = "Other-9";
					print(id, "Other-9");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Num0 || e.key.code == sf::Keyboard::Numpad0)
				{
					out[id] = "Foothold";
					print(id, "Foothold");
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Right)
					next = true;
				else if (e.key.code == sf::Keyboard::Left) 
				{
					hold -= 2;
					next = true;
				}
				else if (e.key.code == sf::Keyboard::BackSpace) 
				{
					out.erase(id);
					std::cout << "removed " << id << "\n";
					next = true;
				}
				else if (e.key.code == sf::Keyboard::Up) 
				{
					++face;
					currentTex.loadFromFile("data/imgs/" + data["holds"][face]["face"].get<std::string>() + ".jpg");
					hold = -1;
					next = true;
					routeId = 0;
					loadclimb(data, face, &routeId, route);
				}
				else if (e.key.code == sf::Keyboard::Down) 
				{
					if (--face < 0) face = 0;
					currentTex.loadFromFile("data/imgs/" + data["holds"][face]["face"].get<std::string>() + ".jpg");
					hold = -1;
					next = true;
					routeId = 0;
					loadclimb(data, face, &routeId, route);
				}
				else if (e.key.code == sf::Keyboard::Space)
				{
					std::stringstream ss2(data["holds"][face]["holds"][hold]["centroidStr"].get<std::string>());
					sf::Vector2f centre;
					ss2 >> centre.x >> centre.y;

					view.setCenter(centre);
				}
				else if (e.key.code == sf::Keyboard::Return)
				{
					std::ofstream o("classification.json");
					o << out;
					std::cout << "saved\n";
				}
				else if (e.key.code == sf::Keyboard::H)
				{
					// print current hold
					if (out.find(id) == out.end())
						std::cout << id << " = NONE\n";
					else
						std::cout << id << " = " << out[id] << "\n";
				}
				else if (e.key.code == sf::Keyboard::P) 
				{
					// print debug stuff out
					std::cout << "Size: " << out.size() << "\n";
				}
				else if (e.key.code == sf::Keyboard::R)
				{
					// toggle route
					displayRoute = !displayRoute;
				}
				else if (e.key.code == sf::Keyboard::M)
				{
					++routeId;
					loadclimb(data, face, &routeId, route);
				}
				else if (e.key.code == sf::Keyboard::N)
				{
					--routeId;
					loadclimb(data, face, &routeId, route);
				}

				if (next) {
					++hold;
					int tmp = face;
					loadhold(data, &face, &hold, currentTex, sprite, va, view);
					if (tmp != face)
					{
						routeId = 0;
						loadclimb(data, face, &routeId, route);
					}
				}
			}
		}

		if (pressed) {
			sf::Vector2i curPos = sf::Mouse::getPosition(window);
			view.move(sf::Vector2f(lastPos - curPos) * zoom);
			lastPos = curPos;
		}

		window.setView(view);

		window.clear();

		window.draw(sprite);

		if (displayRoute)
		{
			for (sf::VertexArray& v : route) 
			{
				window.draw(v);
			}
		}
		else
			window.draw(va);

		window.display();
	}

	std::ofstream o("classification.json");
	o << out;

	return 0;
}