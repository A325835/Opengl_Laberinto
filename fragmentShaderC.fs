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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    
    if (int(UseTexture) == 0)
   {
     FragColor = Color;
   }
   else{
        //luz ambiental
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));


        //luz difusa 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));


        //luz especular 
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir),0.0f), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
   
     
    
        //muestra la ambiental con la difusa haciendo el shader phong sin material
        vec3 result = ambient + diffuse + specular;

        FragColor = vec4(result,1.0);
    }
}