#version 330

#define EPS 1.0e-5

in vec3 f_posCameraSpace;

out vec4 out_color;

uniform mat4 u_mvMat;
uniform vec3 u_volumeSize;
uniform float u_volumeResolution;
uniform float u_threshold;
uniform sampler3D u_volumeTexture;

float pitch = u_volumeResolution / 2.0;

vec3 calcNormal(vec3 uvw, float eps) {
	float x0 = texture(u_volumeTexture, vec3(uvw.x - eps, uvw.y, uvw.z)).x; 
	float x1 = texture(u_volumeTexture, vec3(uvw.x + eps, uvw.y, uvw.z)).x; 
	float dx = (x1 - x0) / (2.0 * eps);
	float y0 = texture(u_volumeTexture, vec3(uvw.x, uvw.y - eps, uvw.z)).x; 
	float y1 = texture(u_volumeTexture, vec3(uvw.x, uvw.y + eps, uvw.z)).x; 
	float dy = (y1 - y0) / (2.0 * eps);
	float z0 = texture(u_volumeTexture, vec3(uvw.x, uvw.y, uvw.z - eps)).x; 
	float z1 = texture(u_volumeTexture, vec3(uvw.x, uvw.y, uvw.z + eps)).x; 
	float dz = (z1 - z0) / (2.0 * eps);
	return -normalize(vec3(dx, dy, dz));
}

vec3 rayMarching(vec3 vMin, vec3 vMax,vec3 org, vec3 dir) {
	float tMin = -1.0e12;
	float tMax = 1.0e12;
	for(int i=0;i<3;i++){
		if(abs(dir[i])<EPS){
			if(org[i]<vMin[i]||vMax[i]<org[i]){
				return vec3(0.0,0.0,0.0);
			}
			continue;
		}
		float tmpMin = (vMin[i] - org[i]) / dir[i];
		float tmpMax = (vMax[i] - org[i]) / dir[i];
		if (tmpMin > tmpMax) {
			float t = tmpMin;
			tmpMin = tmpMax;
			tmpMax = t;
		}
		tMin = max(tmpMin,tMin);
		tMax = min(tmpMax,tMax);
	}
	if(tMin>tMax){
		return vec3(0.0,0.0,0.0);
	}

	for (int i = int(tMin); i < int(tMax); i++) {
		vec3 p = (org + dir * float(i)) / u_volumeResolution;
		vec3 uvw = p + u_volumeSize / 2.0;
		for(int j=0;j<3;j++){
			uvw[j] /= u_volumeSize[j];
		}
		if (uvw.x < 0.0 || uvw.y < 0.0 || uvw.z < 0.0) {
			continue;
		}
		if (uvw.x > 1.0 || uvw.y > 1.0 || uvw.z > 1.0) {
			continue;
		}
		float v = float(texture(u_volumeTexture, uvw).x);
		if (v > u_threshold) {
			vec3 n = calcNormal(uvw, pitch / 10.0);
			//float NdotL = max(0.1, dot(n, vec3(0.0, 1.0, 0.0)));
			//return vec3(NdotL, NdotL, NdotL);
			return n * 0.5 + 0.5;
		}
	}
	return vec3(0.0);
}

void main(void) {
	vec4 temp = inverse(u_mvMat) * vec4(0.0, 0.0, 0.0, 1.0); 
	vec3 org = temp.xyz / temp.w;
	vec3 dir = normalize((inverse(u_mvMat) * vec4(f_posCameraSpace, 0.0)).xyz) * pitch;
	vec3 vMin = -u_volumeSize / 2.0 * u_volumeResolution;
	vec3 vMax = u_volumeSize / 2.0 * u_volumeResolution;

	out_color = vec4(rayMarching(vMin, vMax, org, dir), 1.0);
}
