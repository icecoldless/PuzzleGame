#include "Puzzle.h"
#include "Game.h"
Puzzle::Puzzle(Game* game, Vector2 pos, const string& texPath)
	:Actor(game)
{
	//// Initialize to random position/orientation
	SetPosition(pos);

	SetRotation(0.f);

	//// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture(texPath));

	mc = new MoveComponent(this);
	//// Add to Puzzle in game
	game->AddPuzzle(this);
}

Puzzle::~Puzzle()
{
	GetGame()->RemovePuzzle(this);
}

void Puzzle::Move(Vector2 pos)
{
	mc->Update(pos);
}
