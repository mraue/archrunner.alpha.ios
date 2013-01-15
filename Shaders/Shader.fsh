//
//  Shader.fsh
//  gameone
//
//  Created by Martin Raue on 1/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

precision highp float;

varying lowp vec4 colorVarying;
varying lowp float intensityVarying;

void main()
{
//    vec4 lowp diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
    gl_FragColor = colorVarying;
//    vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
//    gl_FragColor = intensityVarying * vec4(1.0, 0.4, 1.0, 1.0);
//    vec4 color;
//    float intensity = intensityVarying;
//	if (intensity > 0.95)
//		color = vec4(1.0,0.5,0.5,1.0);
//	else if (intensity > 0.5)
//		color = vec4(0.6,0.3,0.3,1.0);
//	else if (intensity > 0.25)
//		color = vec4(0.4,0.2,0.2,1.0);
//	else
//		color = vec4(0.2,0.1,0.1,1.0);
//	gl_FragColor = color;
}
