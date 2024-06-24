#version 330 core

//out vec4 outColor;
in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica

void main()
{
    //outColor = vec4(1.0, 0.0, 0.0, 1.0); // Siva boja za mre�u
   // outColor = vec4(1.0, 0.0, 1.0, 1.0);
    //outColor = vec4(0.0, 0.0, 0.0, 1.0);
    outCol = texture(uTex, chTex); //boja na koordinatama chTex teksture vezane na teksturnoj jedinici uTex
	//mijesanje 2 teksture se moze raditi sa mix(T1, T2, k) funkcijom gdje su
	//T1 i T2 pozivi texture funkcije a k koeficijent jacine druge teksture od 0 do 1 (0.3 = 70%T1 + 30%T2)
	//mijesanje sa bojom tjemena se moze odraditi mnozenjem vektora boja i tekstura
}