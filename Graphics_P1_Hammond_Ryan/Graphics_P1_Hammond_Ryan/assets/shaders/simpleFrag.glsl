#version 400

// FRAGMENT shader

in vec3 diffuseColour;
in vec4 vertWorldXYZ;
in vec4 vertNorm;

uniform bool bUseDebugDiffuseNoLighting;

uniform vec3 myLightPosition;		// In the world
uniform vec3 myLightDiffuse;		// 1,1,1 (white colour light)
uniform float lightConstAtten;// = 0.0f;		// Can set to 1.0 to "turn off" a light
uniform float lightLinAtten;// = 0.1f;
uniform float lightQuadAtten;// = 0.01f;
// For spec:
uniform vec3 eyeLocation;

out vec4 out_Colour;

void main()
{
	// Check to see if we are going to do the lighthing calculation
	if ( bUseDebugDiffuseNoLighting )
	{	// Bypass lighting entirely and set to uniform "debug" diffuse
		out_Colour.xyz = diffuseColour.xyz;
		out_Colour.a = 1.0f;
		return;
	}

	vec3 lightv = myLightPosition - vertWorldXYZ.xyz;
	
	float distanceFromLight = length(lightv); 
//	float lightConstAtten = 0.0f;		// Can set to 1.0 to "turn off" a light
//	float lightLinAtten = 0.1f;
//	float lightQuadAtten = 0.01f;
	
//	float linAtten = (1.0f/distanceFromLight) * lightLinAtten;  
	
//	float quadAtten = (1.0f/(distanceFromLight*distanceFromLight))
//	                    * lightQuadAtten;

	float atten = 1.0f /( lightConstAtten + 
	                      lightLinAtten * distanceFromLight +
						  lightQuadAtten * distanceFromLight * distanceFromLight );
	
	lightv = normalize(lightv);
	
 	vec3 diffuse = max(0.0, dot(lightv, vertNorm.xyz)) 
                   * diffuseColour
				   * myLightDiffuse;
				   	
	// Calculate the linear attenuation of the diffuse
//	diffuse.xyz *= linAtten;	
	diffuse *= atten;

	
	// With newer (we're using version 4 of GLSL) some of the 
	// 	built in variables are depricated, like gl_FragColor
	//gl_FragColor = vec4(diffuse, 1.0f);	
	
	// Bump up the brightness (projector is too dark)
	diffuse.xyz *= vec3(1.5f);
	
	out_Colour = vec4(diffuse, 1.0f);	
	//out_Colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	out_Colour *= 0.001f;	// make it (essentially) zero.	
//	out_Colour += vec4(myLightDiffuse.xyz, 1.0f);
}