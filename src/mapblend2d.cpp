/******************************************************************************
* $Id$
*
* Project:  MapServer
* Purpose:  Blend Rendering functions
* Author:   Thomas Bonfort
*
******************************************************************************
* Copyright (c) 1996-2009 Regents of the University of Minnesota.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies of this Software or works derived from this Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*****************************************************************************/
#include "iostream"
#include <map>
#include <tuple>
#include "mapblend2d.h"
#include <blend2d.h>
#include "mapserver.h"
#include "cpl_conv.h"
#include "fontcache.h"
#include <string.h>

typedef struct blender {
  BLImage * img;
  BLContext * context;
  std::map<std::tuple<int, int, int>, BLRgba> *colorLookup;
} blender;

static BLRgba colorObjToBlendRgba(blender * context, colorObj * c) {
  // not considering alpha
//  std::tuple<int, int, int> key = {c->red, c->green, c->blue};

//  if (context->colorLookup->count(key) > 0) {
//    return (*context->colorLookup)[key];
//  }

  auto newColor = BLRgba(c->red/ 255.0, c->green/ 255.0, c->blue/255.0, c->alpha /255.0);
//  (*context->colorLookup)[key] = newColor;
  return newColor;
}


static blender * imgContextToBlendContext(imageObj *img) {
  return (blender*)img->img.plugin;
}


void initializeCache(void **vcache) {

}

int cleanupBlend(void *cache) {

  return MS_SUCCESS;
}


int freeImageBlend(imageObj *img) {

  return MS_SUCCESS;
}



/**
 *
 * @param mapserverLineCap
 * @return
 */
static uint32_t mapserverLineCapToBlendLineCap(int mapserverLineCap) {
  switch (mapserverLineCap) {
    case MS_CJC_BUTT:
      return BL_STROKE_CAP_BUTT;
      break;
    case MS_CJC_SQUARE:
      return BL_STROKE_CAP_SQUARE;
      break;
    case MS_CJC_ROUND:
      return BL_STROKE_CAP_ROUND;
      break;
    default:
      msDebug("Unrecognised or unsupported MapServer line cap option. Reverting to default line cap.");
      return BL_STROKE_CAP_BUTT;
  }
}

int renderLineBlend(imageObj *img, shapeObj *p, strokeStyleObj *stroke) {
  auto blend = imgContextToBlendContext(img);

  auto color = colorObjToBlendRgba(blend,stroke->color);
  BLStrokeOptions options{};
  auto linecap = mapserverLineCapToBlendLineCap(stroke->linecap);

  options.startCap = linecap;
  options.endCap = linecap;
  blend->context->setStrokeOptions(options);

  blend->context->setStrokeStyle(color);
  blend->context->setStrokeWidth(stroke->width);

  for (int i = 0; i < p->numlines; i++) {
    BLPath path;

    lineObj *l = &(p->line[i]);
    path.moveTo(l->point[0].x, l->point[0].y);

    for (int j = 1; j < l->numpoints; j++) {
      path.lineTo(l->point[j].x, l->point[j].y);
    }

    blend->context->strokePath(path);
  }
  return MS_SUCCESS;
}

int renderPolygonTiledBlend(imageObj *img, shapeObj *p, imageObj *tile) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderPolygonTiledBlend()");
  return MS_FAILURE;
}

int renderPixmapSymbolBlend(imageObj *img, double x, double y,
                            symbolObj *symbol, symbolStyleObj *style) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderPixmapSymbolBlend()");
  return MS_FAILURE;
}

int renderVectorSymbolBlend(imageObj *img, double x, double y,
                            symbolObj *symbol, symbolStyleObj *style) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderVectorSymbolBlend()");
  return MS_FAILURE;
}

int renderSVGSymbolBlend(imageObj *img, double x, double y, symbolObj *symbol,
                         symbolStyleObj *style) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderSVGSymbolBlend()");
  return MS_FAILURE;
}

int renderTileBlend(imageObj *img, imageObj *tile, double x, double y) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderTileBlend()");
  return MS_FAILURE;
}

int renderGlyphs2Blend(imageObj *img, const textSymbolObj *ts, colorObj *c,
                       colorObj *oc, int ow, int isMarker) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderGlyphs2Blend()");
  return MS_FAILURE;
}

imageObj *createImageBlend(int width, int height, outputFormatObj *format,
                           colorObj *bg) {
  imageObj *image = NULL;

  image = (imageObj *)calloc(1, sizeof(imageObj));
  BLContextCreateInfo createInfo {};

  createInfo.threadCount = 1;
  auto renderer = new BLImage(width, height, BL_FORMAT_PRGB32);

  auto ctx = new BLContext(); // heap allocate

  auto beginResult = ctx->begin(*renderer, createInfo);

  if (beginResult != BL_SUCCESS) {
    msSetError(MS_MISCERR,"Could not begin blend context", "createImageBlend()");
    return NULL;
  }

  blender * x = NULL;
  x = (blender *)calloc(1, sizeof(x));
  x->img = renderer;
  x->context = ctx;
  image->img.plugin = x;
  x->colorLookup = new std::map<std::tuple<int, int, int>, BLRgba>();
  return image;
}

int saveImageBlend(imageObj *img, mapObj *map, FILE *fp,
                   outputFormatObj *format_unused) {
  msSetError(MS_MISCERR, "Not implemented",
             "saveImageBlend()");
  return MS_FAILURE;
}

int renderEllipseSymbolBlend(imageObj *img, double x, double y,
                             symbolObj *symbol, symbolStyleObj *style) {
  msSetError(MS_MISCERR, "Not implemented",
             "renderEllipseSymbolBlend()");
  return MS_FAILURE;
}


int startLayerRasterBlend(imageObj *img, mapObj *map, layerObj *layer) {
  auto blend = imgContextToBlendContext(img);
  blend->context->save();
  return MS_SUCCESS;
}

int closeLayerRasterBlend(imageObj *img, mapObj *map, layerObj *layer) {

  auto blend = (blender*)img->img.plugin;
  blend->context->restore();

  return MS_SUCCESS;
}

int getRasterBufferHandleBlend(imageObj *img, rasterBufferObj *rb) {

  auto blend = (blender*)img->img.plugin;
  blend->context->end(); // tell blend to flush
  rb->type = MS_BUFFER_BYTE_RGBA;
  auto imgData = new BLImageData{}; // TODO: You don't clean this up!!!!

  blend->img->getData(imgData);

  auto *pb = (unsigned char *)imgData->pixelData;
  rb->data.rgba.pixels = (unsigned char *)imgData->pixelData;
  rb->data.rgba.row_step = imgData->stride;
  rb->data.rgba.pixel_step = 4;
  rb->width = imgData->size.w;
  rb->height = imgData->size.h;

  rb->data.rgba.r = &(pb[2]);
  rb->data.rgba.g = &(pb[1]);
  rb->data.rgba.b = &(pb[0]);

  rb->data.rgba.a = &(pb[3]);

  return MS_SUCCESS;

}

int getRasterBufferCopyBlend(imageObj *img, rasterBufferObj *rb) {
  msSetError(MS_MISCERR, "Not implemented",
             "getRasterBufferCopyBlend()");
  return MS_FAILURE;
}

int blendCompositeRasterBuffer(imageObj *img, rasterBufferObj *rb,
                               CompositingOperation comp, int opacity) {
  msSetError(MS_MISCERR, "Not implemented",
             "blendCompositeRasterBuffer()");
  return MS_FAILURE;
}

int mergeRasterBufferBlend(imageObj *img, rasterBufferObj *rb, double opacity,
                           int srcX, int srcY, int dstX, int dstY, int width,
                           int height) {

  msSetError(MS_MISCERR, "Not implemented",
             "mergeRasterBufferBlend()");
  return MS_FAILURE;

}


int freeSymbolBlend(symbolObj *s) {
  msSetError(MS_MISCERR, "Not implemented",
             "freeSymbolBlend()");
  return MS_FAILURE;
}

int renderPolygonBlend(imageObj *img, shapeObj *p, colorObj *c) {

  auto blend = imgContextToBlendContext(img);

  auto currentColor = colorObjToBlendRgba(blend,c);

  blend->context->setFillStyle(currentColor);

//  for (int i = 0; i < p->numlines; i++) {
//    BLPath path;
//    lineObj *l = &(p->line[i]);
//    path.moveTo(l->point[0].x, l->point[0].y);
//    for (int j = 0; j < l->numpoints; j++) {
//      path.lineTo(l->point[j].x, l->point[j].y);
//    }
//
//    blend->context->fillPath(path);
//  }

  for (int i = 0; i < p->numlines; i++) {
    BLArray<BLPoint> points;
    lineObj *l = &(p->line[i]);

    for (int j = 0; j < l->numpoints; j++) {
      points.append(BLPoint(l->point[j].x, l->point[j].y));
    }

    blend->context->fillPolygon(points.view());
  }

  return MS_SUCCESS;
}

int msPopulateRendererVTableBlend(rendererVTableObj *renderer) {
  renderer->supports_pixel_buffer = 1;
  renderer->compositeRasterBuffer = blendCompositeRasterBuffer;
  renderer->supports_svg = 1;
  renderer->default_transform_mode = MS_TRANSFORM_SIMPLIFY;
  initializeCache(&MS_RENDERER_CACHE(renderer));
  renderer->startLayer = startLayerRasterBlend;
  renderer->endLayer = closeLayerRasterBlend;
  renderer->renderLineTiled = NULL;
  renderer->renderLine = &renderLineBlend;
  renderer->createImage = &createImageBlend;
  renderer->saveImage = &saveImageBlend;
  renderer->getRasterBufferHandle = &getRasterBufferHandleBlend;
  renderer->getRasterBufferCopy = &getRasterBufferCopyBlend;
  renderer->renderPolygon = &renderPolygonBlend;
  renderer->renderGlyphs = &renderGlyphs2Blend;
  renderer->freeImage = &freeImageBlend;
  renderer->renderEllipseSymbol = &renderEllipseSymbolBlend;
  renderer->renderVectorSymbol = &renderVectorSymbolBlend;
  renderer->renderSVGSymbol = &renderSVGSymbolBlend;
  renderer->renderPixmapSymbol = &renderPixmapSymbolBlend;
  renderer->mergeRasterBuffer = &mergeRasterBufferBlend;
  renderer->renderTile = &renderTileBlend;
  renderer->loadImageFromFile = &msLoadMSRasterBufferFromFile;
  renderer->renderPolygonTiled = &renderPolygonTiledBlend;
  renderer->freeSymbol = &freeSymbolBlend;
  renderer->cleanup = &cleanupBlend;
  return MS_SUCCESS;
}
