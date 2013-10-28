/**
 * @file
 * mnstri.glsl.vignette node implementation.
 *
 * @copyright Copyright © 2013 Månsteri
 * This code may be modified and distributed under the terms of the MIT License.
 */

#include "node.h"

#include "VuoImageRenderer.h"

VuoModuleMetadata({
                     "title" : "GLSL Vignette Image Filter",
                     "description" : "Vignette image filter",
                      "keywords" : [ "glsl", "filter", "vignette" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						 "VuoImageRenderer"
					 ],
					 "node": {
						 "isInterface" : false
					 }
				 });

#include "node.h"

static const char * fragmentShaderSource = VUOSHADER_GLSL_SOURCE(120,
    // Inputs
    uniform sampler2D texture;
    uniform float innerRadius;
    uniform float outerRadius;
    varying vec4 fragmentTextureCoordinate;

    void main()
    {
        vec4 Color = texture2D(texture, fragmentTextureCoordinate.xy);
          float dist = distance(fragmentTextureCoordinate.xy, vec2(0.5,0.5));
          Color.rgb *= smoothstep(outerRadius, innerRadius, dist);
          gl_FragColor = Color;
    }
);


struct nodeInstanceData
{
    VuoShader shader;
    VuoImageRenderer imageRenderer;
};

struct nodeInstanceData * nodeInstanceInit(void)
{
    struct nodeInstanceData * instance = (struct nodeInstanceData *)malloc(sizeof(struct nodeInstanceData));
    VuoRegister(instance, free);

    instance->shader = VuoShader_make("Swap Color Channels", VuoShader_getDefaultVertexShader(), fragmentShaderSource);
    VuoRetain(instance->shader);
    instance->imageRenderer = VuoImageRenderer_make();
    VuoRetain(instance->imageRenderer);

    return instance;
}

void nodeInstanceEvent
(
        VuoInstanceData(struct nodeInstanceData *) instance,
        VuoInputData(VuoImage) image,
        //VuoInputData(VuoReal, {"default":1,"suggestedMin":0,"suggestedMax":1,"suggestedStep":0.1}) red,
        //VuoInputData(VuoReal, {"default":1,"suggestedMin":0,"suggestedMax":1,"suggestedStep":0.1}) green,
        //VuoInputData(VuoReal, {"default":1,"suggestedMin":0,"suggestedMax":1,"suggestedStep":0.1}) blue,
        //VuoInputData(VuoReal, {"default":0, "suggestedStep":0.01}) time,
        //VuoInputData(VuoPoint2d, {"default":{"x":0,"y":0}}) mouse,
        //VuoInputData(VuoPoint2d, {"default":{"x":1024,"y":768}}) resolution,
        VuoInputData(VuoReal, {"default":0.3, "suggestedStep":0.01}) innerRadius,
        VuoInputData(VuoReal, {"default":1.0, "suggestedStep":0.01}) outerRadius,
        VuoOutputData(VuoImage) adjustedImage
)
{
    if (! image)
        return;

    // Associate the input image with the shader.
    VuoShader_resetTextures((*instance)->shader);
    VuoShader_addTexture((*instance)->shader, image, "texture");

    // Feed parameters to the shader.
    /*
    VuoShader_setUniformFloat((*instance)->shader, "red", red);
    VuoShader_setUniformFloat((*instance)->shader, "green", green);
    VuoShader_setUniformFloat((*instance)->shader, "blue", blue);
    VuoShader_setUniformFloat((*instance)->shader, "time", time);
    VuoShader_setUniformPoint2d((*instance)->shader, "mouse", mouse);
    */
    VuoShader_setUniformFloat((*instance)->shader, "innerRadius", innerRadius);
    VuoShader_setUniformFloat((*instance)->shader, "outerRadius", outerRadius);
    //VuoShader_setUniformPoint2d((*instance)->shader, "resolution", radius);

    // Render.
    *adjustedImage = VuoImageRenderer_draw((*instance)->imageRenderer, (*instance)->shader, image->pixelsWide, image->pixelsHigh);
}

void nodeInstanceFini(VuoInstanceData(struct nodeInstanceData *) instance)
{
    VuoRelease((*instance)->shader);
    VuoRelease((*instance)->imageRenderer);
}