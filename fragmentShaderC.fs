#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;
in float UseTexture;
in vec3 Normal;
in vec3 FragPos;

in VS_OUT {
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

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

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
    
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    vec3 normal = texture(normalMap, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 color = texture(diffuseMap, TexCoords).rgb;

    // Obtenemos la dirección de la vista
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculamos el factor de desplazamiento (offset) basado en la dirección de la vista
    float heightScale = 0.05; // Ajusta esto según el efecto deseado
    float height = texture(normalMap, TexCoords).r * heightScale; // Suponemos que la información de profundidad está en el canal rojo del normal map
    vec2 offset = viewDir.xy * height;

    // Aplicamos el offset al TexCoords para modificar la apariencia del normal map
    vec2 newTexCoords = TexCoords + offset;

    // Obtenemos el nuevo normal modificado por el desplazamiento
    vec3 modifiedNormal = texture(normalMap, newTexCoords).rgb;
    modifiedNormal = normalize(modifiedNormal * 2.0 - 1.0);

    if(theta>light.cutOut)
    {
        //ambiental
        vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords)) * color;
        // diffuse 
        vec3 norm = Normal;
        float diff = max(dot(norm, lightDir), 0.0);
        ///////////normal
        vec3 lightDir_tangent = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        diff += max(dot(lightDir_tangent, modifiedNormal), 0.0);
        /////////
        vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        /////normal
        vec3 viewDir_tangent = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
        vec3 reflectDir_tangent = reflect(-lightDir_tangent, modifiedNormal);
        vec3 halfwayDir = normalize(lightDir_tangent + viewDir_tangent);  
        spec += pow(max(dot(modifiedNormal, reflectDir_tangent), 0.0), 32.0);
        ///////////
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = (-light.cutOut + light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;

        float distance = length(light.position - FragPos);
        float atenuacion = 1.0f/(light.constant + light.lineal * distance + light.quatratic * (distance * distance));

        ambient *= atenuacion;
        diffuse *= atenuacion;
        specular *= atenuacion;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
        
    }
    else
    {
        vec3 result = light.ambient  * vec3(texture(material.diffuse, TexCoords)) * color;
        FragColor = vec4(result, 1.0);
    }    
}