
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

static  SDL_Renderer *  renderer;
static  SDL_Surface *   surface;
static  SDL_Texture *   target;
static  TTF_Font *      font;
static  SDL_Texture *   iconTexture = nullptr;

static  void  tellError(unsigned uLine)
{
  ::SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED (@%u): %s", uLine, ::SDL_GetError());
}

static  SDL_Surface * createDirectFontSurface(TTF_Font * pFont, const char * pcszText)
{
  SDL_Color sc  = { 0xAF, 0xAF, 0xAF, 0xAF };
  SDL_Surface * pSdlSurface = ::TTF_RenderUTF8_Blended(pFont, pcszText, sc);
  if(pSdlSurface == nullptr)
  {
    ::SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED (@%u): %s", __LINE__, ::TTF_GetError());
  }
  return pSdlSurface;
}

static  SDL_Texture *   getTexture(SDL_Surface * pSdlSurface)
{
  SDL_Texture * pSdlTexture = nullptr;
  if(pSdlSurface != nullptr)
  {
    pSdlTexture = ::SDL_CreateTextureFromSurface(renderer, pSdlSurface);
    if(pSdlTexture == nullptr)
    {
      tellError(__LINE__);
    }
  }
  return pSdlTexture;
}

static  SDL_Texture *   createDirectFontTexture(TTF_Font * pFont, const char * pcszText)
{
  SDL_Surface * pSdlSurface = createDirectFontSurface(pFont, pcszText);
  if(pSdlSurface != nullptr)
  {
    SDL_Texture * pSdlTexture = getTexture(pSdlSurface);
    ::SDL_FreeSurface(pSdlSurface);
    return pSdlTexture;
  }
  return nullptr;
}

static  void  drawSimpleIcon(const SDL_Rect& rcPos)
{
  if(iconTexture != nullptr)
  {
    ::SDL_SetTextureColorMod(iconTexture, 0xAD, 0xD8, 0xE6);
    ::SDL_SetTextureAlphaMod(iconTexture, 0xFF);
    ::SDL_RenderCopy(renderer, iconTexture, nullptr, & rcPos);
  }
}

static  void  drawSimpleText(const SDL_Rect& rcPos)
{
  SDL_Texture * pSdlTexture = createDirectFontTexture(font, "A");
  if(pSdlTexture != nullptr)
  {
    SDL_Rect  rc;
    if(::SDL_QueryTexture(pSdlTexture, nullptr, nullptr, & rc.w, & rc.h) == 0)
    {
      rc.x = rcPos.x;
      rc.y = rcPos.y;
      ::SDL_SetTextureAlphaMod(pSdlTexture, 0xFF);
      ::SDL_RenderCopy(renderer, pSdlTexture, nullptr, & rc);
    }
    ::SDL_DestroyTexture(pSdlTexture);
  }
}

static  void  drawIcon(const SDL_Rect& rcPos)
{
  drawSimpleIcon(rcPos);
  drawSimpleText(rcPos);
  SDL_Rect  rc  = rcPos;
  rc.x--;
  rc.y--;
  rc.w++;
  rc.h++;
  rc.w++;
  rc.h++;
  ::SDL_SetRenderDrawColor(renderer, 0xCD, 0xCD, 0xCD, 0x3F);
  ::SDL_RenderDrawRect(renderer, & rc);
}

static  void  drawIconSelected(const SDL_Rect& rcPos)
{
  drawIcon(rcPos);
  ::SDL_SetRenderDrawColor(renderer, 0xDC, 0xDC, 0xDC, 0xFF);
  SDL_Rect  rc  = rcPos;
  rc.x--;
  rc.y--;
  rc.w++;
  rc.h++;
  rc.w++;
  rc.h++;
  ::SDL_RenderDrawRect(renderer, & rc);
  rc.x--;
  rc.y--;
  rc.w++;
  rc.h++;
  rc.w++;
  rc.h++;
  ::SDL_RenderDrawRect(renderer, & rc);
}

static  void  drawIconSelectedFill(const SDL_Rect& rcPos)
{
  drawIconSelected(rcPos);
  ::SDL_SetRenderDrawColor(renderer, 0xAD, 0xD8, 0xE6, 0xFF);
  SDL_Rect  rc  = rcPos;
  rc.x  -= 4;
  rc.w  = 4;
  ::SDL_RenderFillRect(renderer, & rc);
}

int main(int, char * *)
{
  SDL_Window *  window;

  if(::SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    tellError(__LINE__);
    return 2;
  }
  if(! ::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"))
  {
    tellError(__LINE__);
    return 2;
  }

#ifdef  DEDICATED_TARGET_DESKTOP
  window  = ::SDL_CreateWindow("GW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
#else
  window  = ::SDL_CreateWindow("GW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
#endif
  if(window == nullptr)
  {
    tellError(__LINE__);
    return 2;
  }
  renderer  = ::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if(renderer == nullptr)
  {
    tellError(__LINE__);
    return 2;
  }

  ::SDL_Delay(40);

  int idi;
  SDL_DisplayMode sdm;
  SDL_RendererInfo sri;
  int rw, rh;
  int lw, lh;

  idi  = ::SDL_GetWindowDisplayIndex(window);

  ::SDL_GetWindowDisplayMode(window, &sdm);
  ::SDL_GetRendererOutputSize(renderer, & rw, & rh);

#ifdef  DEDICATED_TARGET_DESKTOP
  rw = 480;
  rh = 800;
#endif

  ::SDL_RenderSetLogicalSize(renderer, rw, rh);

  idi  = ::SDL_GetWindowDisplayIndex(window);

  ::SDL_GetWindowDisplayMode(window, &sdm);
  ::SDL_GetRendererOutputSize(renderer, & rw, & rh);
  ::SDL_GetRendererOutputSize(renderer, & lw, & lh);
  ::SDL_GetRendererInfo(renderer, &sri);
  ::SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

  ::SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  ::SDL_RenderSetViewport(renderer, nullptr);

  SDL_Rect rcBckg;
  ::SDL_RenderGetViewport(renderer, &rcBckg);
  ::SDL_RenderFillRect(renderer, &rcBckg);

  ::SDL_RenderClear(renderer);
  ::SDL_RenderPresent(renderer);

  ::SDL_Delay(40);

  target = ::SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_TARGET, rw, rh);
  ::SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
  Uint32  u32Format;
  int     iAccess;
  int     iw, ih;
  ::SDL_QueryTexture(target, &u32Format, &iAccess, &iw, &ih);

  ::SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

  if(::TTF_Init() != 0)
  {
    tellError(__LINE__);
    return(2);
  }

#ifdef  DEDICATED_TARGET_DESKTOP
  #define ASSETS  "/tmp/"
#else
  #define ASSETS
#endif
  font  = ::TTF_OpenFont(ASSETS "Arimo-Bold.ttf", 30);
  if (!font) {
    tellError(__LINE__);
    return(2);
  }

  surface = IMG_Load( ASSETS "something.png");
  iconTexture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!iconTexture) {
    tellError(__LINE__);
    return(2);
  }
  ::SDL_FreeSurface(surface);

  ::SDL_SetWindowTitle(window, "Window");

  ::SDL_Delay(40);

  int done  = 0;
  while (!done) {
    SDL_Event event;
    while (::SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        done = 1;
    }

    ::SDL_SetRenderTarget(renderer, target);

    ::SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    ::SDL_RenderClear(renderer);

    SDL_Rect  rcPos = { rw / 8, rh / 16, rw / 10, rw / 10 };

    drawSimpleIcon(rcPos);
    rcPos.y += rcPos.h;
    drawSimpleText(rcPos);
    rcPos.y += rcPos.h;

    drawIcon(rcPos);
    rcPos.y += rcPos.h;
    drawIcon(rcPos);
    rcPos.y += rcPos.h;
    drawIcon(rcPos);
    rcPos.y += rcPos.h;
    drawIcon(rcPos);
    rcPos.y += rcPos.h;
    drawIcon(rcPos);
    rcPos.y += rcPos.h;
    drawIconSelectedFill(rcPos);
    rcPos.y += rcPos.h;

    drawIcon(rcPos);
    rcPos.x += rcPos.w;
    drawIcon(rcPos);
    rcPos.x += rcPos.w;
    drawIcon(rcPos);
    rcPos.x += rcPos.w;
    drawIcon(rcPos);
    rcPos.x += rcPos.w;
    drawIcon(rcPos);
    rcPos.x += rcPos.w;
    drawIconSelected(rcPos);
    rcPos.x += rcPos.w;

    rcPos = { rw / 8 + rw / 2, rh / 16, rw / 10, rw / 10 };
    drawSimpleIcon(rcPos);

    ::SDL_SetRenderTarget(renderer, nullptr);
    ::SDL_RenderCopy(renderer, target, nullptr, nullptr);
    ::SDL_RenderPresent(renderer);

    ::SDL_Delay(10000);
  }

  ::SDL_DestroyTexture(iconTexture);

  ::TTF_CloseFont(font);

  ::SDL_DestroyRenderer(renderer);
  ::SDL_DestroyWindow(window);

  ::TTF_Quit();
  ::SDL_Quit();

  return(0);
}
