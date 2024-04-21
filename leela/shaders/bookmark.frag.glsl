#version 450 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 offset;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = vec3(1.0, 0.1, 0.1);
    
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse
    // Frustum is expected to be ortho.
    // With respect to the monitor screen:
    //      * +z is perpendicular and into the screen.
    //      * +x is to the right.
    //      * +y goes directly up.
    vec3 lightPos = vec3(1000, 1000, -2000) + offset;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    //vec3 viewPos = vec3(0, 0, -1000);
    //float specularStrength = 0.5;
    //vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * spec * lightColor;
    
    //vec3 result = (ambient + diffuse + specular) * objectColor;
    
    vec3 temp = (ambient + diffuse) * objectColor;
    
    float excess = 0;
    if (temp.x > 1.0)
        excess = temp.x - 1;
    else if (temp.y > 1.0)
        excess = temp.y - 1;
    else if (temp.z > 1.0)
        excess = temp.z - 1;
    
    // add excess to all components
    temp = temp + excess;
    
    vec3 result = temp;
    FragColor = vec4(result, 1.0);
}
