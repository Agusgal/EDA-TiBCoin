#include <iostream>
#include "App/App.h"

//#define DEMO
#define APP


#ifdef APP
int main()
{
	try 
	{
		App myApp;
		Events ev;

		while (myApp.isRunning()) 
		{
			ev = myApp.eventGenerator();
			
			myApp.dispatcher(ev);
		}
	}
	catch (std::exception& e) 
	{
		std::cout << e.what() << std::endl;
	}
}
#endif //APP







#ifdef DEMO

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "../Lib/ImGui/imgui.h"
#include "../Lib/ImGui/imgui_impl_allegro5.h"

int main(int, char**)
{
	// ---------- Inicializacion de Allegro. ----------
	al_init();

	// Dear ImGui necesita Allegro para poder graficar. Para esto, usa el add-on de primitivas.
	al_init_primitives_addon();

	// El display usado es el de Allegro. Se registran sus eventos para poder cerrarlo 
	// apretando 'X' o para poder cambiar su tamanio en ejecucion.
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	al_register_event_source(queue, al_get_display_event_source(display));

	// Tambien necesita Allegro para recibir eventos del usuario, por lo que hay que instalar 
	// el mouse y el teclado y registrar sus eventos.
	al_install_keyboard();	al_register_event_source(queue, al_get_keyboard_event_source());
	al_install_mouse();		al_register_event_source(queue, al_get_mouse_event_source());

	// Setup de Dear ImGui
	IMGUI_CHECKVERSION();			// Control de version de Dear ImGui
	ImGui::CreateContext(); ImGui_ImplAllegro5_Init(display); // Inicializa Dear ImGui
	ImGuiIO& io = ImGui::GetIO();	// Me permite obtener informacion y cambiar parametros del mouse y de teclado.
									// Por ejemplo, puedo cambiar la velocidad a la que se repiten los caracteres 
									// cuando mantengo una tecla presionada, o puedo saber si el evento de 
									// mouse/teclado que acaba de llegar fue dentro de una ventana de ImGui o fuera.

	ImGui::StyleColorsLight();		// Opcional: cambiar la paleta de colores default por otra

	bool running = true;			// true hasta que se cierre el display.
	bool keep_open = true;			// true hasta que se cierre la ventana a la que se asigna.

	//variables auxiliares que usan los widgets
	bool show_another_window = false;
	bool show_demo_window = false;
	unsigned int counter = 0;
	float f = 0.0f;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	unsigned int intensity = 0;
	bool checkbox_A_selected = false;

	while (running)
	{
		ALLEGRO_EVENT ev;
		while (al_get_next_event(queue, &ev))
		{
			ImGui_ImplAllegro5_ProcessEvent(&ev);	// Mandar el evento a Dear ImGui para que lo procese

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				running = false;
			if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
			{
				ImGui_ImplAllegro5_InvalidateDeviceObjects();
				al_acknowledge_resize(display);
				ImGui_ImplAllegro5_CreateDeviceObjects();
			}

			// io.WantCaptureKeyboard devuelve true si el ultimo evento fue de teclado y ademas fue 
			// usado por el usuario para interactuar con las ventanas de Dear Imgui. Por este motivo, 
			// sabemos que no estaba dirigido a nuestra aplicacion principal y no debemos mandarselo a 
			// su dispatcher. Idem para io.WantCaptureMouse y los eventos de mouse
			if (!io.WantCaptureKeyboard && !io.WantCaptureMouse)
			{
				// Dispatch a nuestra aplicacion principal 
			}
		}

		// Inicio el frame. Se realiza una vez por cada pantalla que dibujo.
		ImGui_ImplAllegro5_NewFrame();
		ImGui::NewFrame();

		////////////////////////////////////////////////////////////////////////
		//Ejemplos de uso de widgets (sacado del repo principal de DearImGui) //
		////////////////////////////////////////////////////////////////////////

// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}




		// Rendering
		ImGui::Render();	//No dibuja! Solamente calcula que es lo que tiene que dibujarse

		// Puedo usar funciones de dibujo de Allegro ademas de las de 
		// ImGui.
		// Todo lo que dibuje antes de ImGui_ImplAllegro5_RenderDrawData
		// va a quedar detras de las ventanas de DearImGui, y todo lo 
		// que dibuje despues va a quedar encima de las ventanas de 
		// DearImGui.

		al_clear_to_color(al_map_rgba_f(1, 1, 0.8, 1));	//Va a quedar detras de las ventanas.

		//Todo lo que dibuje aca va a quedar por detrás de las ventanas de DearImGui

		ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());	//Dibuja las ventanas, pero no hace al_flip_display()

		//Todo lo que dibuje aca va a quedar por encima de las ventanas de DearImGui

		al_flip_display(); //DearImGui nunca hace al_flip_display()


	}

	// Cleanup final
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	al_shutdown_primitives_addon();

	return 0;
}
#endif