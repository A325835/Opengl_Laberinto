#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;
in float UseTexture;
in vec3 Normal;
in vec3 FragPos;

//uniform sampler2D texture_diffese1;
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
     vec3 position;
    vec3 direction;
    float cutOut;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float lineal;
    float quatratic;
};



uniform vec3 viewPos;
uniform Material material;
uniform Light light;

//void main()
//{
//    if (int(UseTexture) == 0)
//    {
//        FragColor = Color;
//    }
//    else
//    {
//       FragColor = texture(kk, TexCoords);
//    }
//}


void main()
{
    
     vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    
    
    if (int(UseTexture) == 0)
   {
     FragColor = Color;
   }
   else{
   if(theta>light.cutOut)
   {
        //ambiental
        vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = (light.cutOut - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;

        float distance = length(light.position - FragPos);
        float atenuacion = 1.0f/(light.constant + light.lineal
        * distance + light.quatratic * (distance * distance));

        ambient *= atenuacion;
        diffuse *= atenuacion;
        specular *= atenuacion;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
        
    }
    else
    {
        vec3 result = light.ambient  * vec3(texture(material.diffuse, TexCoords));
        FragColor = vec4(result, 1.0);
    }    
   }
}