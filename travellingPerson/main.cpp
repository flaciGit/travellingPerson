#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>
#include <time.h>

GLsizei winWidth = 800, winHeight = 600;

GLint pointsDropped = 0;
GLint maxPointsDropped = 4;
GLint gameState = 0;

vec2 houseStartCoord;
bool foundHouseStartCoord = false;

GLfloat pointLineSize = 3;
GLfloat pointSize = 5;

GLint houseWidth = 50;

vec2 personCoords[2];
GLint secondLegRadius = 15;
GLint personBodyLength = 30;
GLint personHeadWidth = 10;
GLint counterForTheLeg = 0;
GLint personLineWidth = 2;

GLint dbPointToTravel = 0;
GLint personRightAtThisPoint = 0;
GLint personLeftAtThisPoint = 0;
bool inicDbPointToTravel = true;
bool personArrived = false;
bool movementStarted = false;
GLint keyStates[256];

GLfloat t0 = -1;
GLfloat t1 = 0;
GLfloat t2 = 1;
GLfloat t3 = 2;

GLfloat bgR = 0.1;
GLfloat bgG = 1.0;
GLfloat bgB = 1.0;

GLfloat bgGDarkener = 0.005;
GLfloat bgGDarkenerLimit = 0.15;

GLint glutTimerFuncValue = 30;

GLint dbStar = 10;
GLint starSize = 3;

vec2 points[4] = { {-10, -10 }, { -10, -10 }, { -10, -10 }, { -10, -10 } };

GLint dragged = -1;

void init()
{
	srand(time(NULL));
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE);
	glPointSize(pointSize);
	glLineWidth(5.0);
	glLineStipple(1, 0xFF00);
}

void keyPressed(unsigned char key, int x, int y)
{
	keyStates[key] = 1;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStates[key] = 0;
}

void keyOperations(int value)
{
	if (keyStates[32]) { //32 space
		movementStarted = true;
		keyStates[32] = 0;
	}
	if (keyStates['w']) {
		movementStarted = true;
		keyStates['w'] = 0;
	}
	glutPostRedisplay();

	glutTimerFunc(glutTimerFuncValue, keyOperations, 0);
}


void goodNight() {

	GLint c = 0;
	while (c < dbStar) {

		GLint pointX = rand() % winWidth;
		GLint pointY = rand() % winHeight/4 + winHeight * 3 / 4;

		glPointSize(starSize);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_POINTS);
		glVertex2f(pointX, pointY);
		glEnd();

		c++;
	}
	
}

void hermiteCurve() {

	mat24 G = { points[0],points[1],points[2],points[3] };

	vec4 o0 = { t0*t0*t0, t0*t0, t0, 1 };
	vec4 o1 = { t1*t1*t1, t1*t1, t1, 1 };
	vec4 o2 = { t2*t2*t2, t2*t2, t2, 1 };
	vec4 o3 = { t3*t3*t3, t3*t3, t3, 1 };

	mat4 N = { o0,o1, o2, o3, true };

	mat4 M = inverse(N);

	mat24 C = G * M;

	glPointSize(pointLineSize);
	glBegin(GL_POINTS);
	
	GLint stepCounter = 0;
	for (GLdouble t = -1; t <= 2; t += 0.01) {

		vec4 TI = { (float)(t*t*t), (float)(t*t), (float)t, 1 };
		vec2 P = C * TI;
		glVertex2f(P.x, P.y);

		float diffX = points[3].x - P.x;
		float diffY = points[3].y - P.y;

		if (!foundHouseStartCoord && houseWidth*houseWidth >= (diffY * diffY) + (diffX *diffX))
		{
			houseStartCoord = { P.x, P.y };
			foundHouseStartCoord = true;
		}

		diffX = personCoords[0].x - P.x;
		diffY = personCoords[0].y - P.y;

		if (secondLegRadius*secondLegRadius >= (diffY * diffY) + (diffX *diffX)) {
			personCoords[1] = { P.x, P.y };
		}
		if (movementStarted) {
			if (personRightAtThisPoint == stepCounter) {

				personCoords[1] = { P.x, P.y };
				if (bgG > bgGDarkenerLimit)
					bgG -= bgGDarkener;
			}

			if (personLeftAtThisPoint == stepCounter) {

				personCoords[0] = { P.x, P.y };
			}
			stepCounter++;
		}
		
		if (inicDbPointToTravel) {
			dbPointToTravel++;
		}
		
	}
	inicDbPointToTravel = false;

	if (movementStarted && personRightAtThisPoint < dbPointToTravel) {
		personRightAtThisPoint++;
		personLeftAtThisPoint++;
	}
	if (personRightAtThisPoint >= dbPointToTravel) {
		personArrived = true;
	}

	glEnd();
	glPointSize(pointSize);

}

void circle(GLfloat x, GLfloat y, GLdouble r) {
	glBegin(GL_LINE_LOOP);
	for (GLdouble t = 0; t <= 2 * pi(); t += 0.01)
		glVertex2d(x + r * cos(t), y + r * sin(t));

	glEnd();

}

void drawPerson() {

	glColor3f(0.0, 0.0, 0.0);
	
	glPointSize(personLineWidth);
	glLineWidth(personLineWidth);

	glBegin(GL_LINES);
	GLfloat personX1Temp = personCoords[1].x - personCoords[0].x;
	GLfloat personY1Temp = personCoords[1].y - personCoords[0].y;

	GLfloat n = sqrt(personX1Temp*personX1Temp + personY1Temp * personY1Temp);
	personX1Temp /= n;
	personY1Temp /= n;

	GLfloat personX1 = -personY1Temp;
	GLfloat personY1 = personX1Temp;
	

	glVertex2f(personX1*personBodyLength + personCoords[0].x, personY1*personBodyLength + personCoords[0].y);
	glVertex2f(personCoords[0].x, personCoords[0].y);

	glVertex2f(personX1*personBodyLength/2 + personCoords[0].x, personY1*personBodyLength/2 + personCoords[0].y);
	glVertex2f(personCoords[1].x, personCoords[1].y);

	glEnd();
	
	circle(personX1*(personBodyLength+personHeadWidth) + personCoords[0].x,
		personY1*(personBodyLength+ personHeadWidth) + personCoords[0].y,
		personHeadWidth
		);

	glBegin(GL_LINES);

	personX1Temp = personCoords[0].x - personCoords[1].x;
	personY1Temp = personCoords[0].y - personCoords[1].y;

	n = sqrt(personX1Temp*personX1Temp + personY1Temp * personY1Temp);
	personX1Temp /= n;
	personY1Temp /= n;

	personX1 = personY1Temp;
	personY1 = -personX1Temp;

	glVertex2f(personX1*personBodyLength * 3 / 4 + personCoords[0].x, personY1*personBodyLength * 3 / 4 + personCoords[0].y);
	glVertex2f(personX1*personBodyLength*3/4 + personCoords[1].x, personY1*personBodyLength * 3 / 4 + personCoords[1].y);

	glEnd();

}
void drawHouse() {

	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(pointLineSize);
	glBegin(GL_LINES);


	glVertex2f(houseStartCoord.x, houseStartCoord.y);
	glVertex2f(points[3].x, points[3].y);

	GLfloat x1Temp = points[3].x - houseStartCoord.x;
	GLfloat y1Temp = points[3].y - houseStartCoord.y;

	GLfloat x1 = -y1Temp;
	GLfloat y1 = x1Temp;

	glVertex2f(x1 + houseStartCoord.x, y1 + houseStartCoord.y);
	glVertex2f(houseStartCoord.x, houseStartCoord.y);

	GLfloat x2Temp = houseStartCoord.x - points[3].x;
	GLfloat y2Temp = houseStartCoord.y - points[3].y;

	GLfloat x2 = y2Temp;
	GLfloat y2 = -x2Temp;

	glVertex2f(x2 + points[3].x, y2 + points[3].y);
	glVertex2f(points[3].x, points[3].y);

	glVertex2f(x1 + houseStartCoord.x, y1 + houseStartCoord.y);
	glVertex2f(x2 + points[3].x, y2 + points[3].y);

	GLfloat n = sqrt(x1Temp*x1Temp + y1Temp * y1Temp);
	GLfloat x1N = x1Temp / n;
	GLfloat y1N = y1Temp / n;
	
	GLfloat x3 = x1N * cos(pi() * 45 / 180) - y1N * sin(pi() * 45 / 180);
	GLfloat y3 = y1N * cos(pi() * 45 / 180) + x1N * sin(pi() * 45 / 180);

	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(x1 + houseStartCoord.x, y1 + houseStartCoord.y);
	
	GLfloat lengthOfTriangle = sqrt(houseWidth* houseWidth + houseWidth * houseWidth) / 2;
	glVertex2f(x3 *lengthOfTriangle + houseStartCoord.x+x2, y3*lengthOfTriangle + houseStartCoord.y+y2);
	

	glVertex2f(x3 *lengthOfTriangle + houseStartCoord.x + x2, y3*lengthOfTriangle + houseStartCoord.y + y2);
	glVertex2f(x2 + points[3].x, y2 + points[3].y);

	if (personArrived) {

		glColor3f(1.0, 1.0, 0.6);

		GLfloat lengthOfTriangle1 = sqrt(houseWidth* houseWidth + houseWidth * houseWidth) / 3;
		GLfloat lengthOfTriangle2 = sqrt(houseWidth* houseWidth + houseWidth * houseWidth) * 2 / 3;

		glVertex2f(x3 *lengthOfTriangle1 + houseStartCoord.x , y3*lengthOfTriangle1 + houseStartCoord.y);
		
		GLfloat x2N = x2Temp / n;
		GLfloat y2N = y2Temp / n;

		GLfloat x4 = x2N * cos(pi() * 45 / 180) - y1N * sin(pi() * 45 / 180);
		GLfloat y4 = y2N * cos(pi() * 45 / 180) + x1N * sin(pi() * 45 / 180);

		glVertex2f(x4 *lengthOfTriangle2 + points[3].x, y4*lengthOfTriangle2 + points[3].y);

		glVertex2f(x3 *lengthOfTriangle1 + houseStartCoord.x, y3*lengthOfTriangle1 + houseStartCoord.y);
		glVertex2f(x4 *lengthOfTriangle1 + points[3].x, y4*lengthOfTriangle1 + points[3].y);
		
		glVertex2f(x3 *lengthOfTriangle2 + houseStartCoord.x, y3*lengthOfTriangle2 + houseStartCoord.y);
		glVertex2f(x4 *lengthOfTriangle1 + points[3].x, y4*lengthOfTriangle1 + points[3].y);

		glVertex2f(x3 *lengthOfTriangle2 + houseStartCoord.x, y3*lengthOfTriangle2 + houseStartCoord.y);
		glVertex2f(x4 *lengthOfTriangle2 + points[3].x, y4*lengthOfTriangle2 + points[3].y);

	}

	glEnd();

}

void display()
{
	GLint i;
	glClear(GL_COLOR_BUFFER_BIT);

	
	if (personRightAtThisPoint > dbPointToTravel / 2) {
		goodNight();
	}
	
	glClearColor(bgR,bgG,bgB, 0.0);
	
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; i++)
		glVertex2f(points[i].x, points[i].y);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);

	if (pointsDropped == maxPointsDropped) {
		hermiteCurve();
		if (foundHouseStartCoord) {
			drawHouse();
			if(!personArrived)
				drawPerson();
		}
	}
	glutSwapBuffers();
}


void update(int n)
{
	glutPostRedisplay();
	glutTimerFunc(glutTimerFuncValue, update, 0);
}

GLint getActivePoint1(vec2 p[], GLint size, GLint sens, GLint x, GLint y)
{
	GLint i, s = sens * sens;
	vec2 P = { (float)x, (float)y };

	for (i = 0; i < size; i++)
		if (dist2(p[i], P) < s)
			return i;
	return -1;
}

GLint getActivePoint2(vec2 *p, GLint size, GLint sens, GLint x, GLint y)
{
	GLint i;
	for (i = 0; i < size; i++)
		if (fabs((*(p + i)).x - x) < sens && fabs((*(p + i)).y - y) < sens)
			return i;
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLint i;

	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && gameState == 0
		&& pointsDropped < maxPointsDropped) {
		points[pointsDropped] = { (float)xMouse,(float)winHeight -yMouse };
		pointsDropped++;
		if (pointsDropped == maxPointsDropped) {
			personCoords[0] = points[0];
			gameState++;
		}
			
	}
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && gameState == 1)
		if ((i = getActivePoint1(points, 4, 8, xMouse, winHeight - yMouse)) != -1) {
			dragged = i;
			printf("i: %d, points[%d]:[%g,%g] \n",i,i,points[i].x, points[i].y);
			
		}
			
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;
}

void processMouseActiveMotion(GLint xMouse, GLint yMouse)
{
	GLint i;
	if (dragged >= 0) {
		points[dragged].x = xMouse;
		points[dragged].y = winHeight - yMouse;
		foundHouseStartCoord = false;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Drag & Drop");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutTimerFunc(0, update, 0);
	glutTimerFunc(0, keyOperations, 0);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutMainLoop();
	return 0;
}


