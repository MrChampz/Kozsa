#version 330 core

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;
in vec3 Color;

out vec4 FragColor;

///////////////////////////////////////
// Material Parameters
///////////////////////////////////////
uniform samplerCube prefilterMap;
uniform samplerCube irradianceMap;
uniform sampler2D	brdfLUT;

uniform sampler2D texture_albedo1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_albedo2;
uniform sampler2D texture_normal2;
uniform sampler2D texture_albedo3;
uniform sampler2D texture_normal3;
uniform sampler2D texture_albedo4;
uniform sampler2D texture_normal4;

///////////////////////////////////////
// Lights
///////////////////////////////////////
uniform vec3 lightPositions[14];
uniform vec4 lightColors[14];

uniform vec3 camPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
	vec3 albedo;
    float roughness;
	float metallic = 0.0f;
    float ao = 1.0f;

	vec3 N;
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N);

	// -------------------- SLOPE --------------------
	float slope = 1.0 - Normal.y;

	// Determine which material to use based on slope.
	if(slope < 0.5)
	{
		float blendAmount = slope / 0.2f;

		albedo 		= mix(
        	pow(texture(texture_albedo2, TexCoords).rgb, vec3(2.2)),
        	pow(texture(texture_albedo1, TexCoords).rgb, vec3(2.2)),
        	blendAmount);

		roughness 	= mix(
        	texture(texture_albedo1, TexCoords).a,
        	texture(texture_albedo1, TexCoords).a,
        	blendAmount);

		N			= mix(
        	normalize(TBN * (texture(texture_normal2, TexCoords).xyz * 2.0 - 1.0)),
			normalize(TBN * (texture(texture_normal1, TexCoords).xyz * 2.0 - 1.0)),
        	blendAmount);
	}
	else if(slope >= 0.5) 
	{
		albedo 		= pow(texture(texture_albedo1, TexCoords).rgb, vec3(2.2));
		roughness 	= texture(texture_albedo1, TexCoords).a;
		N			= normalize(TBN * (texture(texture_normal1, TexCoords).xyz * 2.0 - 1.0));
	}
	// -----------------------------------------------
	// ----------------- BLEND MAP -------------------
	float redAmount		= Color.r;
	float greenAmount	= Color.g;

	if (redAmount > 0.0f)
	{
		vec3  rAlbedo		= pow(texture(texture_albedo3, TexCoords).rgb, vec3(2.2));
		float rRoughness	= texture(texture_albedo3, TexCoords).a;
		vec3  rNormal		= normalize(TBN * (texture(texture_normal3, TexCoords).xyz * 2.0 - 1.0));

		albedo		= mix(albedo, rAlbedo, redAmount);
		roughness	= mix(roughness, rRoughness, redAmount);
		N			= mix(N, rNormal, redAmount);
	}
	if (greenAmount > 0.0f)
	{
		vec3  gAlbedo		= pow(texture(texture_albedo4, TexCoords).rgb, vec3(2.2));
		float gRoughness	= texture(texture_albedo4, TexCoords).a;
		vec3  gNormal		= normalize(TBN * (texture(texture_normal4, TexCoords).xyz * 2.0 - 1.0));

		albedo		= mix(albedo, gAlbedo, greenAmount);
		roughness	= mix(roughness, gRoughness, greenAmount);
		N			= mix(N, gNormal, greenAmount);
	}
	// -----------------------------------------------

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 14; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i].rgb * attenuation;

		// Temp. For directional light
		if (i == 13)
		{
			L = -lightPositions[i];
			H = normalize(V + L);
			radiance = lightColors[i].rgb;
		}

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
		vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 brdf = nominator / denominator;

         // kS is equal to Fresnel
        vec3 kS = F;

        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;

        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
    }   

    // ambient lighting
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;
	
	const float MAX_REFLECTION_LOD = 5.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));

    // gamma correct
    color = pow(color, vec3(1.0 / 2.2)); 

    FragColor = vec4(color, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom 	= a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom 	= NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}