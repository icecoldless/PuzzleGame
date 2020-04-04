#pragma once
#include "SDL/SDL.h"
#include<vector>
#include<iostream>
#include <unordered_map>
#include <GL/glew.h>
#include "SpriteComponent.h"
#include "VertexArray.h"
#include "Puzzle.h"
#include <queue>
#pragma warning(disable : 4996)
using namespace std;
class Game {
public:
	Game();
	bool Initialize();
	void RunLoop();
	void ShutDown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void AddPuzzle(class Puzzle* puzzle);
	void RemovePuzzle(class Puzzle* puzzle);

	class Texture* GetTexture(const std::string& fileName);


private:
	void RedisplayWindow(int windex);

	void ProcessInput();
	void UpdateGame();
	void GenerateOutPut();

	bool LoadShaders();
	void CreateSpriteVerts();
	void LoadData(const int);
	void UnloadData();


	//Game Logic
	void Swap(int*, const int, const int);
	void Suffle();
	bool IsOk(int*);
	void BFSA();
	int GetDr(int, int);
	void DisplayChange(int* t);
	string Hash(int*);

	//window
	int wWidths[3];
	int wIndex;

	// Map of textures loaded
	unordered_map<std::string, class Texture*> mTextures;

	// All the actors in the game
	vector<class Actor*> mActors;
	// Any pending actors
	vector<class Actor*> mPendingActors;
	// All the sprite components drawn
	vector<class SpriteComponent*> mSprites;
	//All the Puzzle
	vector<class Puzzle*> mPuzzles;
	// Sprite shader
	class Shader* mSpriteShader;
	// Sprite vertex array
	class VertexArray* mSpriteVerts;

	SDL_Window* mWindow;
	SDL_GLContext mContext;
	Uint32 mTicksCount;
	bool mIsRunning;

	// Track if we're updating actors right now
	bool mUpdatingActors;

	//Game Logic
	int* data;
	Vector2 positions[25];
	int dst[25];
	int dir3[9][4];
	int dir4[16][4];
	int dir5[25][4];

	struct QNode {
		Game* game;
		int data[25]; //数据
		int step, cost,zeroIndex,index,pid;
		int changeId;//父图形变换到这里需要修改的spriteId
		Vector2 zeroPos;  //原先0的位置
		Vector2 movePos; //移动轨迹
		bool operator<(const QNode &a)const {
			return cost > a.cost;
		}//这里限定了不能使用指针！！！！
		QNode() {

		}
		QNode(int* _n, int s,Game* _g,int _pid=-1)
			:index(-1)
		{
			for (int i = 0; i < 25; i++)
			{
				data[i] = _n[i];
			}
			step = s;
			game = _g;
			pid = _pid;
			setCost();
		
		}
		void setCost() {
			int c = 0;
			int w = game->wIndex + 3;
			for (int i = 0; i < w*w; i++)
			{
				if (data[i] == 0)
				{
					zeroIndex = i;
					continue;
				}//略过空格
				//seek 曼哈顿距离
				int ri = data[i] / w;
				int cj = data[i] % w;
				int dri = game->dst[i] / w;
				int dcj = game->dst[i] % w;
				c += Math::Abs(dri - ri) + Math::Abs(dcj - cj);
				//if (data[i] != game->dst[i] && data[i] != 0)
				//{
				//	c++;
				//}
				
			}
			cost = c*5 + step;
		}
	};
	priority_queue<QNode> queue; //队列
	vector<QNode> graphs; //遍历过的所有图形结点
	unordered_map<string, bool> isVisited; //判断结点图形是否访问过
};
