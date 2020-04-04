//#include "Sprite.h"
//Sprite::Sprite()
//	:mWidth(0)
//	,mHeight(0)
//	,IsMove(false)
//{
//}
//void Sprite::Init(SDL_Texture* tex, int _x, int _y)
//{
//	SetPosition(_x, _y);
//	SetTexture(tex);
//}
//void Sprite::Move(int &x,int &y) {
//	if (IsMove)
//	{
//		if (x == 0 && y == 0)
//		{
//			IsMove = false;
//			return;
//		}
//		else if (x > 0)
//		{
//			centerX+= 10;
//			x -= 10;
//		}
//		else if(x<0)
//		{
//			centerX -= 10;
//			x += 10;
//		}
//		else if (y > 0)
//		{
//			centerY += 10;
//			y -= 10;
//		}
//		else if (y < 0)
//		{
//			centerY -= 10;
//			y += 10;
//		}
//	}
//}
//void Sprite::Draw(SDL_Renderer* renderer) {
//	if (mTexture)
//	{
//		SDL_Rect r;
//		r.w = static_cast<int>(mWidth);
//		r.h = static_cast<int>(mHeight);
//		r.y = static_cast<int>(centerX - mWidth / 2);
//		r.x = static_cast<int>(centerY - mHeight / 2);
//		SDL_RenderCopy(renderer, mTexture, nullptr,&r);
//	}
//}
//void Sprite::SetPosition(int _x, int _y) {
//	centerX = _x;
//	centerY = _y;
//}
//void Sprite::SetTexture(SDL_Texture* texture) {
//	mTexture = texture;
//	SDL_QueryTexture(texture, nullptr, nullptr, &mWidth, &mHeight);
//}
