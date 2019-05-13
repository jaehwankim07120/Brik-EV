/**
 * @file display_sdl2.c
 * @author Ben
 * @date  7 Mar 2019
 * @brief
 */
#ifdef DISPLAY_TYPE_SDL2
#include "brik_api.h"
#include "display_sdl.h"

/* ******* STATIC DEFINE ******* */
moduleImageViewer_t moduleImageViewer;
/* ******* GLOBAL VARIABLE ******* */

/* ******* STATIC FUNCTIONS ******* */
/* *** IMAGE *** */

//from YUV frame
static int clean_window(moduleImageViewer_t* module);
static int update_texture(moduleImageViewer_t* module, AVFrame* av_frame);
static int update_frame(moduleImageViewer_t* module, AVFrame* av_frame);

/* * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * *
 *
 *  Extern Functions
 *
 * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * */

 int MODULE_Display_Init(void)
 {
    int ret = 0;

    printf("MODULE INIT => Display handler.\n");

    /* SDL INIT */
    ret = SDL_Init(SDL_INIT_VIDEO);
    if(ret != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "Not Initialized params.");
    }

    /* GET DISPLAY INFO */
    SDL_GetDesktopDisplayMode(0, &moduleImageViewer.dm);

    printf("Image Viwer => Module => Display Resolution [ %d x %d ] %dHz %d\n", moduleImageViewer.dm.w, moduleImageViewer.dm.h, \
                                                                                 moduleImageViewer.dm.refresh_rate, \
                                                                                 moduleImageViewer.dm.format);

    moduleImageViewer.window = SDL_CreateWindow(
             "Brik_EV",
             SDL_WINDOWPOS_UNDEFINED,




             SDL_WINDOWPOS_UNDEFINED,
             moduleImageViewer.dm.w,
             moduleImageViewer.dm.h,
             0
         );

     if (!moduleImageViewer.window) {
         ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "SDL: could not create window - exiting.");
     }

     moduleImageViewer.renderer = SDL_CreateRenderer(moduleImageViewer.window, -1, 0);
     if (!moduleImageViewer.renderer) {
         ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "SDL: could not create renderer - exiting.");
     }
     /* IMAGE INIT */
     clean_window(&moduleImageViewer);

     moduleImageViewer.texture = SDL_CreateTexture(
         moduleImageViewer.renderer,
         SDL_PIXELFORMAT_YV12,
         SDL_TEXTUREACCESS_STREAMING,
         moduleImageViewer.dm.w,
         moduleImageViewer.dm.h
     );

     if (!moduleImageViewer.texture) {
         ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "SDL: could not create texture - exiting.");
     }

     /* Clean Before Window Screen */
     ret = clean_window(&moduleImageViewer);

     return ret;
 }

void MODULE_Display_Clean(void)
{
    clean_window(&moduleImageViewer);
}

int MODULE_Display_Update(AVFrame* av_frame)
{
    return update_frame(&moduleImageViewer, av_frame);
}


int MODULE_Display_Destroy(void)
{
    SDL_DestroyTexture(moduleImageViewer.texture);
    SDL_DestroyRenderer(moduleImageViewer.renderer);
    SDL_DestroyWindow(moduleImageViewer.window);
    SDL_Quit();

    return 0;
}

static int update_frame(moduleImageViewer_t* module, AVFrame* av_frame)
{
    int ret = 0;

    int image_w = 0, image_h = 0;

    /* GET PRINT TARGET INFO */
    ret = update_texture(module, av_frame);
    if (ret == 0)
    {
        SDL_QueryTexture(module->texture, NULL, NULL, &image_w, &image_h);
    }

    module->dest_rect.x = 0;
    module->dest_rect.y = 0;
    module->dest_rect.w = module->dm.w;
    module->dest_rect.h = module->dm.h;

    /* Drawing Image */
    //error += SDL_RenderClear(moduleImageViewer->renderer);
    ret = SDL_RenderCopy(module->renderer, module->texture, NULL, &module->dest_rect);
    if(!ret)
    {
        ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "Get error while copying texture to renderer.");
    }

    SDL_RenderPresent(module->renderer);

    return ret;
}

static int clean_window(moduleImageViewer_t* module)
{
    int ret = 0;
    int error = 0;

    /* Clear */
    error += SDL_RenderClear(module->renderer);
    error += SDL_SetRenderDrawColor(module->renderer, 0, 0, 0, 255);
    if(!error)
    {
        ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "Get error from renderer while clearing.");
    }

    SDL_RenderPresent(module->renderer);

    return ret;
}

/* * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * *
 *
 *  IMAGE
 *
 * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * */

static int update_texture(moduleImageViewer_t* module, AVFrame* av_frame)
{
    int ret = 0;

    if(module->renderer == NULL)
    {
        printf("No renderer found while loading texture\n");
    }

    /* Update Texture*/
    if(module->texture == NULL)
    {
        ERROR_StatusCheck(BRIK_STATUS_NOT_INITIALIZED, "Unable to find the texture to update.");
    }
/*
    SDL_UpdateYUVTexture(
                        moduleI->texture,
                        NULL,
                        av_frame,
                        pCodecCtx->width,
                        uPlane,
                        uvPitch,
                        vPlane,
                        uvPitch
                    );
*/
    ret = SDL_UpdateTexture(module->texture, NULL, av_frame->extended_data, av_frame->width);

    return ret;
}


#endif
