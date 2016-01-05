#include "GUI.h"
#include "Mesh.h"
#include "UniformGrid.h"
#include "MeshContainer.h"

void GUI::resize3DDisplay(int new_width, int new_height) {
	window_size_x = new_width;
	window_size_y = new_height;
	Display::setDisplaySize(window_size_x, window_size_y);
}

void GUI::mouseClickEvent(int button, int release, int x, int y) {
	
	mouse_button_state[button] = !release;

	if (mouse_button_state[button]) {
		last_mouse_x = x;
		last_mouse_y = y;
		mouseClickedCallback(button, x, y);
	}

	//std::cout << "clicked :" << button << " - " << release << " - " << x << " - " << y << std::endl;

}

void GUI::mouseDragEvent(int x, int y) {

	//std::cout << "moved :" << x << " - " << y << std::endl;

	if (mouse_button_state[MOUSE_MIDDLE_BUTTON]) {
		int dx = x - last_mouse_x;
		int dy = y - last_mouse_y;
		cam_angle_x += dx*angle_precision; cam_angle_y += dy*angle_precision;
		last_mouse_x = x;
		last_mouse_y = y;
	}

	mouseDragCallback(x, y);

}

void GUI::keyPressedEvent(unsigned char key_id, int x, int y) {

	//std::cout << "key pressed: " << key_id << std::endl;

	if (key_id == '-') cam_zoom /= zoom_precision;
	if (key_id == '+') cam_zoom *= zoom_precision;

	keyPressedCallback(key_id);

}

void GUI::idle() {
	glutPostRedisplay();
}


void GUI::init(int argc, char **argv) {
	glutInit(&argc, argv);
}

void GUI::createWindow(std::string title, int size_x, int size_y) {

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	window_size_x = size_x;
	window_size_y = size_y;
	glutInitWindowSize(window_size_x, window_size_y);
	glutCreateWindow(title.c_str());
	Display::initDisplay(0, window_size_x, window_size_y);

}

void GUI::startApp(void (*loop_function)(void), void (*mouseClickedCallback)(int, int, int), void (*mouseDragCallback)(int, int), void (*keyPressedCallback)(unsigned char)) {
	GUI::mouseClickedCallback = mouseClickedCallback;
	GUI::mouseDragCallback = mouseDragCallback;
	GUI::keyPressedCallback = keyPressedCallback;

	glutDisplayFunc(loop_function);
	glutIdleFunc(idle);
	glutMouseFunc(mouseClickEvent);
	glutMotionFunc(mouseDragEvent);
	glutReshapeFunc(resize3DDisplay);
	glutKeyboardFunc(keyPressedEvent);
	glutMainLoop();
}

//Display functions
void GUI::begin3DScene() {
	//first, (re)set the projection
	Display::setProjection(60.0f, 0.1f, 2000.0f);

	//set the camera position
	Display::setCameraFromSphericalCoords(cam_angle_x, cam_angle_y, cam_zoom, cam_target);
	//Display::pushMatrix();

	//clear buffers
	Display::clearAllBuffers();
}

void GUI::end3DScene() {
	glutSwapBuffers();
	glutPostRedisplay();
}

void GUI::drawMesh(Mesh& mesh, bool wire_frame) {


	//ordering of the faces
	static const int order[] = {0, 3, 2,
								2, 3, 1,
								1, 3, 0,
								1, 0, 2};

	Maths::Vector3 vtmp;
	Maths::Vector3 v[4];

	if (wire_frame) {

		Display::setDrawColor(210.0f/255.0f, 0, 0);
		Display::setPointSize(5.0f);
		Display::setLighting(false);
		
		//draw points
		for (unsigned int v = 0 ; v < mesh.particles.size() ; v++) {
			if (mesh.particles[v].fixed){
				Display::setDrawColor(0,0,204);
				Display::drawPoint(mesh.particles[v].pos);
			}
			else{
				Display::setDrawColor(210.0f / 255.0f, 0, 0);
				Display::drawPoint(mesh.particles[v].pos);
			}
		}

		//neighbors
		Display::setDrawColor(1, 1, 1);
		Maths::Vector3 start, end;
		for (unsigned int v = 0 ; v < mesh.particles.size() ; v++) {
			start = mesh.particles[v].pos;
			for (unsigned int n = 0 ; n < mesh.particles[v].neighbors.size() ; n++) {
				end = mesh.particles[v].neighbors[n]->pos;
				Display::drawLine(start, end);
			}
		}

		Display::setLighting(true);

	} else {

		//Display::setDrawColor(1, 1, 1);

		//temporary variables
		Maths::Vector3 vt[3];
		Maths::Vector3 n;

		MeshContainer* mc = mesh.getMeshContainer();

		//VOLUMETRIC MESH
		if (mc->getNbVertPerElem() > 3) {
			//loop over all tetrahedra
			for (int e = 0; e < mc->nb_elements ; e ++) {
				
				//get the 4 vertices
				int i = e*4;
				for (int h = 0 ; h < 4 ; h++) {
					int ind = mc->indices[i + h];
					v[h] = mesh.particles[ind].pos;
				}

				//loop over the 4 faces of the tetrahedra
				for (int j = 0 ; j < 12 ; j += 3) {

					//get the vertices
					vt[0] = v[order[j + 0]];
					vt[1] = v[order[j + 1]];
					vt[2] = v[order[j + 2]];

					//compute normal vector
					n = (vt[1] - vt[0]).crossProduct(vt[2] - vt[0]);
					n.normalise();

					//draw the triangle
					Display::drawTriangle(vt[0], n, vt[1], n, vt[2], n);
				}
			}

		} else {

			//SURFACE MESH
			//loop over all tetrahedra
			for (int e = 0; e < mc->nb_elements ; e ++) {
				
				int s = e*mc->getNbVertPerElem();
				for (int v = 0 ; v < mc->getNbVertPerElem() ; v++) vt[v] = mesh.particles[mc->indices[s + v]].pos;

				//compute normal vector
				n = (vt[1] - vt[0]).crossProduct(vt[2] - vt[0]);
				n.normalise();

				//draw the triangle
				Display::drawTriangle(vt[0], n, vt[1], n, vt[2], n);
				Display::drawTriangle(vt[2], -n, vt[1], -n, vt[0], -n);
			}
		}
	}
}

void drawWireBox(Maths::AxisAlignedBox& box) {

	const Maths::Vector3* corner = box.getAllCorners();

	Display::drawLine(corner[0], corner[1]);
	Display::drawLine(corner[1], corner[2]);
	Display::drawLine(corner[2], corner[3]);
	Display::drawLine(corner[3], corner[0]);

	Display::drawLine(corner[4], corner[5]);
	Display::drawLine(corner[5], corner[6]);
	Display::drawLine(corner[6], corner[7]);
	Display::drawLine(corner[7], corner[4]);

	Display::drawLine(corner[5], corner[1]);
	Display::drawLine(corner[4], corner[2]);
	Display::drawLine(corner[6], corner[0]);
	Display::drawLine(corner[7], corner[3]);

}
/*
void GUI::drawGrid(UniformGrid& grid) {

	Maths::AxisAlignedBox b;
	Maths::Vector3 o = grid.getOrigin();

	Maths::Vector3 cell_size;
	cell_size.x = grid.getSize().x/(float)((int)grid.getNbCells().x);
	cell_size.y = grid.getSize().y/(float)((int)grid.getNbCells().y);
	cell_size.z = grid.getSize().z/(float)((int)grid.getNbCells().z);

	for (int x = 0 ; x < grid.getNbCells().x ; x++) {
		for (int y = 0 ; y < grid.getNbCells().y ; y++) {
			for (int z = 0 ; z < grid.getNbCells().z ; z++) {
				
				b.setMinimum(o + Maths::Vector3(cell_size.x*(x + 0), cell_size.y*(y + 0), cell_size.z*(z + 0)));
				b.setMaximum(o + Maths::Vector3(cell_size.x*(x + 1), cell_size.y*(y + 1), cell_size.z*(z + 1)));

				drawWireBox(b);
			}
		}
	}
}
*/

void GUI::beginTransformation(Maths::Vector3 v, Maths::Quaternion q) {

	//build the transformation matrix from vector and quaternion
	Maths::Matrix3 rot;
	q.ToRotationMatrix(rot);

	float mat[16];

	mat[ 0] = rot[0][0]; mat[ 1] = rot[0][1]; mat[ 2] = rot[0][2]; mat[ 3] = 0.0f;
	mat[ 4] = rot[1][0]; mat[ 5] = rot[1][1]; mat[ 6] = rot[1][2]; mat[ 7] = 0.0f;
	mat[ 8] = rot[2][0]; mat[ 9] = rot[2][1]; mat[10] = rot[2][2]; mat[11] = 0.0f;
	mat[12] = v.x      ; mat[13] = v.y      ; mat[14] = v.z      ; mat[15] = 1.0f;

	Display::pushMatrix();
	Display::multMatrix(mat);

}

void GUI::endTransformation() {
	Display::popMatrix();
}

int GUI::window_size_x = 0;
int GUI::window_size_y = 0;

float GUI::cam_angle_x = 0.7f;
float GUI::cam_angle_y = 0.6f;
float GUI::cam_zoom = 9.0f;
Maths::Vector3 GUI::cam_target = Maths::Vector3::ZERO;
float GUI::zoom_precision = 0.85f;
float GUI::angle_precision = 1.0f/200.0f;

int GUI::last_mouse_x;
int GUI::last_mouse_y;

bool GUI::mouse_button_state[3] = {false, false, false};

void (*GUI::mouseClickedCallback)(int, int, int);
void (*GUI::mouseDragCallback)(int, int);
void (*GUI::keyPressedCallback)(unsigned char);
