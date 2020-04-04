#include "Game.h"
#include<stack>
Game::Game()
	:mWindow(nullptr)
	, mSpriteShader(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
	,wWidths{300,400,500}
	,wIndex(0)
	,dst{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24}
	, dir3{
		{-1,-1,1,3},{0,-1,2,4},{1,-1,-1,5},
		{-1,0,4,6},{3,1,5,7},{4,2,-1,8},
		{-1,3,7,-1},{6,4,8,-1},{7,5,-1,-1}
	}
	, dir4{
		{-1,-1,1,4},{0,-1,2,5},{1,-1,3,6},{2,-1,-1,7},
		{-1,0,5,8},{4,1,6,9},{5,2,7,10},{6,3,-1,11},
		{-1,4,9,12},{8,5,10,13},{9,6,11,14},{10,7,-1,15},
		{-1,8,13,-1},{12,9,14,-1},{13,10,15,-1},{14,11,-1,-1}
	}
	, dir5{ 
		{-1,-1,1,5},{0,-1,2,6},{1,-1,3,7},{2,-1,4,8},{3,-1,-1,9},
		{-1,0,6,10},{5,1,7,11},{6,2,8,12},{7,3,9,13},{8,4,-1,14},
		{-1,5,11,15},{10,6,12,16},{11,7,13,17},{12,8,14,18},{13,9,-1,19},
		{-1,10,16,20},{15,11,17,21},{16,12,18,22},{17,13,19,23},{18,14,-1,24},
		{-1,15,21,-1},{20,16,22,-1},{21,17,23,-1},{22,18,24,-1},{23,19,-1,-1}
	}
{
}


bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("PuzzleGame",800,500,
		wWidths[wIndex], wWidths[wIndex], SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	// Make sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	LoadData(wIndex+3);

	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::RedisplayWindow(int wi) 
{
	UnloadData();

	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);

	wIndex = wi;

	Initialize();
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutPut();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	else if (keyState[SDL_SCANCODE_1])
	{
		RedisplayWindow(0);
	}
	else if (keyState[SDL_SCANCODE_2])
	{
		RedisplayWindow(1);
	}
	else if (keyState[SDL_SCANCODE_3])
	{
		RedisplayWindow(2);
	}
	else if (keyState[SDL_SCANCODE_Q])
	{
		Suffle();
	}
	else if (keyState[SDL_SCANCODE_W])
	{
		BFSA();
	}
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();
	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		pending->ComputeWorldTransform();
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();
}

void Game::GenerateOutPut()
{
	// Set the clear color to grey
	glClearColor(.68f, .68f, .68f, 1.f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw all sprite components
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Game::ShutDown()
{
	UnloadData();
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}


void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

bool Game::LoadShaders()
{
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(wWidths[wIndex], wWidths[wIndex]);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
	return true;
}

Texture* Game::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Game::AddPuzzle(Puzzle* puzzle) 
{
	mPuzzles.emplace_back(puzzle);
}

void Game::RemovePuzzle(Puzzle* puzzle)
{
	auto iter = std::find(mPuzzles.begin(),
		mPuzzles.end(), puzzle);
	if (iter != mPuzzles.end())
	{
		mPuzzles.erase(iter);
	}
}

void Game::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f,  0.5f, 0.f, 0.f, 0.f, 1.0f, 0.f, 0.f, 1.f, // top left
		 0.5f,  0.5f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, // top right
		 0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}


void Game::LoadData(const int rowcnt)
{
	data = new int[25]{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
	string dir = "Assets/";
	int base = -wWidths[wIndex]/2+50;
	for (int i = rowcnt-1,z=0; i >=0; i--,z++)
	{
		for (int j = 0; j < rowcnt; j++)
		{
			Vector2 pos = Vector2(base + j * 100, base + i * 100);
			positions[z*rowcnt + j] = pos;
			new Puzzle(this, pos, dir + to_string(z*rowcnt + j) + ".png");
		}
	}
	/*cout << "求出position的值:" << endl;
	for (int i = 0; i < rowcnt; i++)
	{
		for (int j = 0; j < rowcnt; j++)
		{
			cout << "(" << positions[rowcnt*i + j].x << "," << positions[rowcnt*i + j].y << "), ";
		}
		cout << endl;
	}*/
}    

void Game::UnloadData()
{
	while (!mPuzzles.empty())
	{
		delete mPuzzles.back();
	}
	while (!mSprites.empty())
	{
		delete mSprites.back();
	}
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	while (!mPendingActors.empty())
	{
		delete mActors.back();
	}
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();
}

void Game::Swap(int* arr, const int a, const int b)
{
	int t = arr[a];
	arr[a] = arr[b];
	arr[b] = t;
}
void Game::Suffle()
{
	int w = wIndex + 3;
	int cnt = 0, zeroPos,num=w*w;
	for (zeroPos = 0; zeroPos < num; zeroPos++)
	{
		if (data[zeroPos] == 0)
			break;
	}
	while (cnt<90)
	{
		//获得一个随机改变的索引
		int dr;
		if (w == 3)
			dr = dir3[zeroPos][rand() % 4];
		else if (w == 4)
			dr = dir4[zeroPos][rand() % 4];
		else if (w==5)
			dr = dir5[zeroPos][rand() % 4];
		if (dr != -1)
		{
			Swap(data,zeroPos, dr);
			zeroPos = dr;
		}
		cnt++;
	}
	//update view
	for (int i = 0; i < num; i++)
	{
		mPuzzles[data[i]]->SetPosition(positions[i]);
	}
	/*int tmp = 0;
	for (int i = 0; i < num; i++)
	{
		tmp++;
		printf("%c ", data[i] + 'A');
		if (tmp >= w)
		{
			cout << endl;
			tmp = 0;
		}
	}*/
}

bool Game::IsOk(int* graph) {
	
	int w = wIndex + 3;
	for (int i = 0; i < w*w; i++)
	{
		if (graph[i] != dst[i])
			return false;
	}
	return true;
}

int Game::GetDr(int zeroPos, int index)
{
	if (wIndex == 0)
		return dir3[zeroPos][index];
	else if (wIndex == 1)
		return dir4[zeroPos][index];
	else if (wIndex == 2)
		return dir5[zeroPos][index];
}
string Game::Hash(int* arr)
{
	string s = "";
	int num = (wIndex + 3)*(wIndex + 3);
	for (int i = 0; i < num; i++)
	{
		char c = char(arr[i] + 'A');

		s += c;
	}
	return s;
}

void Game::DisplayChange(int* arr)
{
	int w = wIndex + 3;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < w; j++)
		{
			printf("%c ", arr[i*w + j] + 'A');
		}
		cout << endl;
	}
	cout << endl;
}

void Game::BFSA() {
	graphs.clear();
	while (!queue.empty())
		queue.pop();
	QNode first = QNode(data, 0, this); //初始第一个结点
	first.index = graphs.size();
	graphs.push_back(first);
	isVisited[Hash(first.data)] = true;
	queue.push(first);
	QNode tmp;
	int cnt = 0;
	mTicksCount = SDL_GetTicks();
	while (!queue.empty())
	{
		tmp = queue.top();
		queue.pop();
		//cout << "弹出" << endl;
		//DisplayChange(tmp.data);
		int pid = tmp.index;
		cnt++;
		if (IsOk(tmp.data))
		{
			cout << "找到最终解，至少需要" << (SDL_GetTicks() - mTicksCount)/1000 << "s时间" << endl;
			cout << "至少需要移动" << tmp.step << "步" << endl;
			cout << "总共搜索" << cnt << "个状态" << endl;
			while (!queue.empty())
				queue.pop();
			break;
		}
		int zeroIndex = tmp.zeroIndex;
		int step = tmp.step;
		for (int i = 0; i < 4; i++)
		{
			int changeId = GetDr(zeroIndex, i);
			if (changeId != -1)
			{
				Swap(tmp.data,changeId, zeroIndex);
				int tarr[25];
				for (int i = 0; i < 25; i++)
				{
					tarr[i] = tmp.data[i];
				}
				if (isVisited.count(Hash(tmp.data)) == false)
				{
					QNode newnode = QNode(tarr, step + 1, this, pid);
					newnode.changeId = tarr[zeroIndex];
					newnode.zeroPos = positions[zeroIndex];
					newnode.movePos = positions[changeId];
					//cout << "压入" << endl;
					//DisplayChange(newnode.data);
					isVisited[Hash(newnode.data)] = true;
					newnode.index = graphs.size();
					graphs.push_back(newnode);
					queue.push(newnode);
				}
				Swap(tmp.data,changeId, zeroIndex); //回溯
			}
		}
	}
	stack<QNode> s;
	while (tmp.pid != -1)
	{
		s.push(tmp);
		tmp = graphs[tmp.pid];
	}
	//cout << "求出的路径为：" << endl;
	while (!s.empty())
	{
		tmp = s.top();
		s.pop();
		/*cout << "上一个状态移动0到新的位置x="<<tmp.movePos.x<<"y="<<tmp.movePos.y << endl;
		cout << "上一个状态移动"<<char(tmp.changeId+'A')<<"到新的位置x="<<tmp.zeroPos.x<<"y="<<tmp.zeroPos.y << endl;
		DisplayChange(tmp.data);*/
		//start move
		float mTickCnt = SDL_GetTicks();
		while (1)
		{
			/*while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCnt + 10))
				;*/
			mTicksCount = SDL_GetTicks();
			//移动一次
			//0
			//mPuzzles[0]->SetPosition(tmp.movePos);
			//mPuzzles[tmp.changeId]->SetPosition(tmp.zeroPos);
			mPuzzles[0]->Move(tmp.movePos);
			mPuzzles[tmp.changeId]->Move(tmp.zeroPos);
			UpdateGame();
			GenerateOutPut();
			if (mPuzzles[0]->GetPosition().x == tmp.movePos.x&&mPuzzles[0]->GetPosition().y == tmp.movePos.y)
				break;
		}
	}
}