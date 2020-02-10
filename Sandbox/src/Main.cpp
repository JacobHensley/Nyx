#include <Nyx.h>
#include "Layers/ViewerLayer.h"
#include "Nyx/Script.h"

struct player {
public:
	int hp;

	player()
		: player(5, 100) {

	}

	player(int ammo, int hitpoints)
		: ammo(ammo), hp(hitpoints) {

	}

	void boost() {
		hp += 10;
	}

	bool shoot() {
		ammo--;
		return false;
	}

	int GetAmmo() {
		return ammo;
	}
private:
	int ammo;
};

#define UserType(name, ...) sol::usertype<name> type = Application::GetLuaState().new_usertype<name>(#name, sol::constructors<__VA_ARGS__>());
#define Bind(name, ...) type[name] = __VA_ARGS__;

int main()
{
	Application app = Application();

	UserType(player, player(), player(int, int));
	Bind("getAmmo", &player::GetAmmo);
	Bind("hp", &player::hp);

	Script script("assets/textures/Script.lua");

	script.Run();

	ViewerLayer* viewer = new ViewerLayer("ViewerLayer");
	app.PushLayer(viewer);

	app.Run();
}