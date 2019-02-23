/*
a,d-вращение сцены по оси X
w,s-вращение сцены по оси Y
e,r-вращение сцены по оси Z
4-перенос сцены оси X "влево"
6-перенос сцены оси X "вправо"
2-перенос сцены оси Y "вниз"
8-перенос сцены оси Y "вверх"
7-перенос сцены оси Z "вперёд"
9-перенос сцены оси Z "назад"
0-увеличение сцены
1-уменьшение сцены
мышка с нажатой кнопкой - вращение камерой вокруг сцены
KEY_UP-приближение камеры
KEY_DOWN-отдаление камеры
KEY_LEFT/RIGHT-переключение камеры между планетами
*/
#include <GL/glut.h>
#include <irrKlang.h>
#include<SOIL2.h>
#include<iostream>
using namespace irrklang;

ISoundEngine *SoundEngine = createIrrKlangDevice();

const double M_PI = 3.14159265358979323846;
const double M_PI_2 = M_PI / 2.0;
const double M_PI_4 = M_PI / 4.0;
const double ts=2*M_PI;

static int slices = 16;
static int stacks = 16;
static int screensize = 1024;

int uvX = 0;
int uvY = 0;
int uvZ = 0;
int uvSc = 0;

GLfloat RotX = 0;
GLfloat RotY = 0;
GLfloat RotZ = 0;

double trX = 0;
double trY = 0;
double trZ = 0;

double scX = 1;
double scY = 1;
double scZ = 1;

double Rsf;
double Rb;
double lx;
double ly;
double lz;
double lcx;
double lcy;
double lcz;
int numcam = -1;

int px;
int py;
bool first = true;
double oangle;
double fiangle;
double speedrot = 0.01;

int numsc = 1;
int cursc = 1;

bool pause = false;
double tp = 0;
double t = 0;

GLuint StarsTexture = 1;
GLuint SunTexture = 2;
GLuint MerkuriiTexture = 3;
GLuint VenusTexture = 4;
GLuint EarthTexture = 5;
GLuint MarsTexture = 6;
GLuint UpiterTexture = 7;
GLuint SaturnTexture = 8;
GLuint UranTexture = 9;
GLuint NeptunTexture = 10;

void loadtexture(GLuint nametex, const char *path) {
	glGenTextures(1, &nametex);
	glBindTexture(GL_TEXTURE_2D, nametex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *data = SOIL_load_image(path, &width, &height, &nrChannels, 0);
	if (data){
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else{
		std::cout << "Failed to load texture" << std::endl;
	}
	SOIL_free_image_data(data);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void loadalltextures() {
	loadtexture(StarsTexture, "images/Stars1.jpg");
	loadtexture(SunTexture, "images/Sun.jpg");
	loadtexture(MerkuriiTexture, "images/Merkurii.jpg");
	loadtexture(VenusTexture, "images/Venus.jpg");
	loadtexture(EarthTexture, "images/Earth.jpg");
	loadtexture(MarsTexture, "images/Mars.jpg");
	loadtexture(UpiterTexture, "images/Upiter.jpg");
	loadtexture(SaturnTexture, "images/Saturn.jpg");
	loadtexture(UranTexture, "images/Uran.jpg");
	loadtexture(NeptunTexture, "images/Neptun.jpg");
}
void renderSphere(double cx, double cy, double cz, double r, int p, GLuint texname){
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texname);
		double theta1 = 0.0;
		double theta2 = 0.0;
		double theta3 = 0.0;
		double ex = 0.0f;
		double ey = 0.0f;
		double ez = 0.0f;
		double px = 0.0f;
		double py = 0.0f;
		double pz = 0.0f;
		if (r < 0)r = -r;
		if (p < 0)p = -p;
		if (p < 4 || r <= 0){
			glBegin(GL_POINTS);
				glVertex3d(cx, cy, cz);
			glEnd();
			return;
		}
		for (int i = 0; i < p / 2; ++i){
			theta1 = i * ts / p - M_PI_2;
			theta2 = (i + 1) * ts / p - M_PI_2;
			glBegin(GL_TRIANGLE_STRIP);
				for (int j = 0; j <= p; ++j){
					theta3 = j * ts / p;
					ex = cos(theta2)*cos(theta3);
					ey = sin(theta2);
					ez = cos(theta2)*sin(theta3);
					px = cx + r * ex;
					py = cy + r * ey;
					pz = cz + r * ez;
					glNormal3d(ex, ey, ez);
					glTexCoord2d(-(j/(double)p), 2*(i+1)/(double)p);
					glVertex3d(px,py, pz);
					ex = cos(theta1)*cos(theta3);
					ey = sin(theta1);
					ez = cos(theta1)*sin(theta3);
					px = cx + r * ex;
					py = cy + r * ey;
					pz = cz + r * ez;
					glNormal3d(ex, ey, ez);
					glTexCoord2d(-(j/(double)p),2*i/(double)p);
					glVertex3d(px,py,pz);
				}
			glEnd();
		}
	glDisable(GL_TEXTURE_2D);
}

void setdefangles() {
	oangle = M_PI_2;
	fiangle = M_PI_2;
	Rsf = Rb;
}
void setlookat() {
	/*lx=Rsf*sin(oangle)*cos(fiangle);
	ly=Rsf*sin(oangle)*sin(fiangle);
	lz=Rsf*cos(oangle);*/
	lx = Rsf*cos(oangle)*cos(fiangle);
	ly = Rsf*sin(oangle);
	lz = Rsf*sin(fiangle)*cos(oangle);
	lcx = -lx;
	lcy = -ly;
	lcz = -lz;
}
void setdefscales() {
	RotX = 0;
	RotY = 0;
	RotZ = 0;
	trX = 0;
	trY = 0;
	trZ = 0;
	scX = 1;
	scY = 1;
	scZ = 1;
	numcam = -1;
}
void setdef() {
	setdefangles();
	setlookat();
	setdefscales();
}
void mouseMove(int x, int y) {
	if (first == true) {
		px = x;
		py = y;
		first = false;
	}
	else {
		double dx = x - px;
		double dy = py - y;
		oangle += dx * speedrot;
		fiangle += dy * speedrot;
		if (oangle > M_PI_2)oangle = M_PI_2;
		if (oangle < -M_PI_2)oangle = -M_PI_2;
		if (fiangle > M_PI)fiangle = M_PI;
		if (fiangle < -M_PI)fiangle = -M_PI;
		setlookat();
		px = x;
		py = y;
	}
}
void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
		if (state == GLUT_UP)first = true;
}

void setlight(GLfloat *pos, GLfloat *color) {
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, color);
	//for (int i = 0; i < 3; i++)color[i] = color[i] - 0.3;
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);
	//for (int i = 0; i < 3; i++)color[i] = color[i] - 0.3;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
}
void renderback(GLuint texname) {
	renderSphere(0, 0, 0, 3 * Rb, 400, texname);
}

double getb(double a, double e) {
	return a * sqrt(1 - e * e);
}
double au = 0.50;
double rs = 0.01*au;
double SunR = 309 * rs;
int speedsc = 100;
double planetpositions[8][6];
int speedsco = speedsc * 180 / M_PI;

void setposstring(int i,double a,double b,double z,double n) {
	planetpositions[i][0] = a * cos(t*n);
	planetpositions[i][1] = b * sin(t*n);
	planetpositions[i][2] = sin(z*M_PI/180)*a*cos(t*n);
}
void setplanetpositions() {
	double a, b, n;
	a = SunR + 0.38*au;//Меркурий
	b = getb(a, 0.21);
	n= speedsc / 88.0;
	setposstring(0, a, b, 3.38, n);
	planetpositions[0][3] = 0.38*rs;
	planetpositions[0][4] =2;
	planetpositions[0][5] = 58.6;
	a = SunR + 0.72*au;//Венера
	b= getb(a, 0.0068);
	n = speedsc / 224.7;
	setposstring(1, a, b, 3.86, n);
	planetpositions[1][3] = 0.95*rs;
	planetpositions[1][4] = 4;
	planetpositions[1][5] = 243.02;
	a = SunR + au;//Земля
	b = getb(a, 0.016);
	n = speedsc / 365.0;
	setposstring(2, a, b, 7.15, n);
	planetpositions[2][3] = rs;
	planetpositions[2][4] = 23;
	planetpositions[2][5] = 1;
	a = SunR + 1.52*au;//Марс
	b = getb(a, 0.093);
	n = speedsc / 686.98;
	setposstring(3, a, b, 5.65, n);
	planetpositions[3][3] = 0.532*rs;
	planetpositions[3][4] = 25;
	planetpositions[3][5] = 1;
	a = SunR + 5.2*au;//Юпитер
	b= getb(a, 0.048);
	n = speedsc / 4332.6;
	setposstring(4, a, b, 6.09, n);
	planetpositions[4][3] = 10.9*rs;
	planetpositions[4][4] = 3.13;
	planetpositions[4][5] = 9.925/24.0;
	a = SunR + 9.52*au;//Сатурн
	b = getb(a, 0.056);
	n = speedsc / 10759.22;
	setposstring(5, a, b, 5.51, n);
	planetpositions[5][3] = 9*rs;
	planetpositions[5][4] = 26.7;
	planetpositions[5][5] = 10.5/24.0;
	a = SunR + 19.2*au;//Уран
	b = getb(a, 0.046);
	n = speedsc / 30688.5;
	setposstring(6, a, b, 6.48, n);
	planetpositions[6][3] = 3.97*rs;
	planetpositions[6][4] = 97.77;
	planetpositions[6][5] = 0.718;
	a = SunR + 30.11*au;//Нептун
	b = getb(a, 0.009);
	n = speedsc / 60182.0;
	setposstring(7, a, b, 6.43, n);
	planetpositions[7][3] = 3.85*rs;
	planetpositions[7][4] = 28.32;
	planetpositions[7][5] = 0.6653;
}
void drawring(double r1, double r2) {
	r1=r1* planetpositions[5][3];
	r2 = r2 * planetpositions[5][3];
	int n = 250;
	double iter = 2 * M_PI / n;
	glBegin(GL_QUAD_STRIP);
	glVertex3f(r1*cos(0), r1*sin(0), 0);
	glVertex3f(r2*cos(0), r2*sin(0), 0);
	for (int i = 1; i <= n; i++) {
		glVertex3f(r1*cos(i*iter), r1*sin(i*iter), 0);
		glVertex3f(r2*cos(i*iter), r2*sin(i*iter), 0);
	}
	glEnd();
}
void renderrings() {
	glPushMatrix();
		glRotatef(45, 0, 0, 1);
		drawring(14.5, 14.75);
		drawring(14.85, 14.95);
		drawring(15.05, 15.25);
		drawring(15.35, 15.5);
		drawring(16.5, 16.7);
		drawring(16.8, 17.0);
		drawring(17.2, 17.3);
		drawring(17.5, 17.7);
		drawring(17.9, 18.1);
		drawring(18.2, 18.3);
		drawring(18.35, 18.5);
		drawring(19.5, 19.65);
		drawring(19.75, 19.95);
	glPopMatrix();
}
void createplanet(int i) {
	glPushMatrix();
		glTranslated(planetpositions[i][0], planetpositions[i][1], planetpositions[i][2]);
		double opt = planetpositions[i][4] * M_PI / 180;
		glRotatef(-planetpositions[i][4], 0, 1, 0);
		if (i == 5)renderrings();
		glRotated((speedsco / planetpositions[i][5]) *t, 0, 0, 1);
		glRotatef(-90, 1, 0, 0);
		if(i==0)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, MerkuriiTexture);
		if(i==1)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, VenusTexture);
		if(i==2)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, EarthTexture);
		if(i==3)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, MarsTexture);
		if(i==4)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, UpiterTexture);
		if(i==5)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, SaturnTexture);
		if(i==6)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, UranTexture);
		if(i==7)renderSphere(0, 0, 0, 10 * planetpositions[i][3], 100, NeptunTexture);
	glPopMatrix();
}
void scene1(double t) {
	glPushMatrix();
	GLfloat pos[] = { 0,0,0,1};
	GLfloat col[] = { 1,1,1,1 };
	setlight(pos, col);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 1, 1);
	renderback(StarsTexture);
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		renderSphere(0, 0, 0, SunR, 100, SunTexture);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
	GLfloat col1[] = { 0.9,0.9,0.9,1 };
	GLfloat col2[] = { 0.5,0.5,0.5,1 };
	GLfloat colblack[] = { 0,0,0,1 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, col1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colblack);
	for (int i = 0; i < 8; i++)createplanet(i);
	glDisable(GL_LIGHT0);
	glPopMatrix();
}
void setnumcam() {
	if (numcam != -1) {
		lcx = 0;
		lcy = 0;
		lcz = 0;
		double l = sqrt(pow(planetpositions[numcam][0],2)+pow(planetpositions[numcam][1],2)+pow(planetpositions[numcam][2],2));
		Rsf = 30 * planetpositions[numcam][3];
		lx = planetpositions[numcam][0]*(1+Rsf/l);
		ly = planetpositions[numcam][1] * (1 + Rsf / l);
		lz = planetpositions[numcam][2] * (1 + Rsf / l);
	}
}
void drawaxes() {
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-100, 0, 0);
	glVertex3f(100, 0, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, -100, 0);
	glVertex3f(0, 100, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, 0, -100);
	glVertex3f(0, 0, 100);
	glEnd();
}
static void resize(int width, int height) {
	float ar = (float)width / (float)height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, ar, 0.001f, 500.0f);
	Rsf = 15*ar;
	Rb = Rsf;
	setdef();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void renderScene(void) {
	if (pause == false)t = t + 0.003;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	setplanetpositions();
	setnumcam();
	gluLookAt(lx, ly, lz, lcx,lcy,lcz, 0, 0, 1);
	glPushMatrix();
		glTranslated(-trX, -trY, -trZ);
		glRotatef(RotX, 1, 0, 0);
		glRotatef(RotY, 0, 1, 0);
		glRotatef(RotZ, 0, 0, 1);
		glScaled(scX, scY, scZ);
		//drawaxes();
		if (cursc == 1)scene1(t);
	glPopMatrix();
	glutSwapBuffers();
}
static void key(unsigned char key, int x, int y)
{
	switch (key){
		case 27:
		case 'q':
			exit(0);
			break;

		case '+':
			slices++;
			stacks++;
			break;

		case '-':
			if (slices > 3 && stacks > 3)
			{
				slices--;
				stacks--;
			}
			break;
		case 'a':
			RotX = RotX + 5;
			if (RotX >= 360)RotX = RotX - 360;
			break;
		case 'd':
			RotX = RotX - 5;
			if (RotX <= -360)RotX = RotX + 360;
			break;
		case 'w':
			RotY = RotY + 5;
			if (RotY >= 360)RotY = RotY - 360;
			break;
		case 's':
			RotY = RotY - 5;
			if (RotY <= -360)RotY = RotY + 360;
			break;
		case 'e':
			RotZ = RotZ + 5;
			if (RotZ >= 360)RotZ = RotZ - 360;
			break;
		case 'r':
			RotZ = RotZ - 5;
			if (RotZ <= -360)RotZ = RotZ + 360;
			break;
		case '0':
			if (uvSc < 5) {
				uvSc++;
				scX = 1.25*scX;
				scY = 1.25*scY;
				scZ = 1.25*scZ;
			}
			break;
		case '1':
			if (uvSc > -5) {
				uvSc--;
				scX = 0.8*scX;
				scY = 0.8*scY;
				scZ = 0.8*scZ;
			}
			break;
		case '4':
			if (uvX < 5) {
				uvX++;
				trX = trX + Rsf / 2.0;
			}
			break;
		case '6':
			if (uvX > -5) {
				uvX--;
				trX = trX - Rsf / 2.0;
			}
			break;
		case '2':
			if (uvY > -5) {
				uvY--;
				trY = trY - Rsf / 2.0;
			}
			break;
		case '8':
			if (uvY < 5) {
				uvY++;
				trY = trY + Rsf / 2.0;
			}
			break;
		case '7':
			if (uvZ < 5) {
				uvZ++;
				trZ = trZ + Rsf / 2.0;
			}
			break;
		case '9':
			if (uvZ > -5) {
				uvZ--;
				trZ = trZ - Rsf / 2.0;
			}
			break;
		case 'p':
			if (cursc > 1) {
				t = 0;
				cursc--;
				setdef();
			}
			break;
		case 'n':
			if (cursc < numsc) {
				t = 0;
				cursc++;
				setdef();
			}
			break;
		case ' ':
			if (pause == false) {
				pause = true;
				tp = t;
			}
			else pause = false;
			break;
	}
	glutPostRedisplay();
}
static void idle(void)
{
	glutPostRedisplay();
}
void processSpecialKeys(int key, int xx, int yy) {
	float fraction = 0.1f;
	switch (key) {
		case GLUT_KEY_F1:
			setdef();
			break;
		case GLUT_KEY_F2:
			oangle = 0;
			fiangle = 0;
			Rsf = Rb / 0.9;
			setlookat();
			setdefscales();
			break;
		case GLUT_KEY_UP:
			Rsf = Rsf * 0.9;
			setlookat();
			break;
		case GLUT_KEY_DOWN:
			Rsf = Rsf/0.9;
			setlookat();
			break;
		case GLUT_KEY_LEFT:
			if(numcam>-1)numcam--;
			else numcam = 7;
			if (numcam == -1)setdef();
			else setnumcam();
			break;
		case GLUT_KEY_RIGHT:
			if (numcam < 7)numcam++;
			else numcam = -1;
			if (numcam == -1)setdef();
			else setnumcam();
			break;
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	SoundEngine->play2D("music.mp3", GL_TRUE);
	glutInitWindowSize(screensize, screensize);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Solar system");
	glutReshapeFunc(resize);
	loadalltextures();
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glClearColor(1, 1, 1, 1);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutMainLoop();
	return EXIT_SUCCESS;
}
