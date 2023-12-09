#include <gtk/gtk.h>

GtkImage *image_display;
GtkWindow * main_window;
GtkWidget *panel_image;
GtkWidget * event_box;

// Fonction de rappel appelée lorsqu'un bouton de la souris est cliqué
static gboolean on_button_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    if (event->type == GDK_BUTTON_PRESS) {
        // Récupérer les coordonnées du clic
        g_print("Clic à la position : (%.2f, %.2f)\n", event->x, event->y);
    }

    // Indiquer que le signal a été traité avec succès
    return TRUE;
}

void find_fixed(GtkWidget *widget, gpointer user_data) {
    if (GTK_IS_FIXED(widget)) {
        g_print("Found GtkFixed!\n");
        gtk_widget_destroy(widget);
        // Vous pouvez également faire d'autres opérations ici si nécessaire
    }

    // Parcourir les enfants récursivement
    gtk_container_foreach(GTK_CONTAINER(widget), find_fixed, user_data);
}

void open_image(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *pixbuf = NULL;
    GError *error = NULL;
    // Sélecteur de fichiers
    GtkWidget *chooser = gtk_file_chooser_dialog_new("Ouvrir une image",
                                                     GTK_WINDOW(main_window),
                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                                     "Annuler", GTK_RESPONSE_CANCEL,
                                                     "Ouvrir", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    // Filtre
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

    // Afficher le sélecteur de fichiers
    if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
        g_print("Fichier sélectionné : %s\n", filename);
        ////////////////////////////////////////////////////////
        pixbuf = gdk_pixbuf_new_from_file (filename, &error);
        if (!error)
        {
            GdkPixbuf *pixbuf_mini = NULL;

            pixbuf_mini = gdk_pixbuf_scale_simple (pixbuf,
                                                   gdk_pixbuf_get_width (pixbuf) / 2,
                                                   gdk_pixbuf_get_height (pixbuf) / 2,
                                                   GDK_INTERP_NEAREST);

            gtk_image_set_from_pixbuf(GTK_IMAGE(image_display), pixbuf_mini);

            g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(on_button_clicked), NULL);
            //find_fixed(GTK_WIDGET(main_window),NULL);
            //gtk_container_add (GTK_CONTAINER(panel_image), image_display);

        }
        else
        {
            g_critical ("%s",error->message);
        }
        //////////////////////////////////////////////////////////
        //gtk_image_set_from_file(GTK_IMAGE(image_display), filename);
        g_free(filename);
    }

    // Détruire le sélecteur de fichiers
    gtk_widget_destroy(chooser);

    //gtk_widget_set_events(image_display, GDK_BUTTON_PRESS_MASK);
    gtk_widget_show_all(GTK_WIDGET(main_window));
}


int main(int argc, char *argv[])
{

    // Initialise GTK
    gtk_init(NULL,NULL);
    GtkBuilder* builder = gtk_builder_new ();

    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "interfaceFinal.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    main_window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    if (main_window == NULL)
    {
        g_print("window == NULL\n");
    }
    GtkWidget *open_item = GTK_WIDGET(gtk_builder_get_object(builder, "open_image"));
    if (open_item == NULL)
    {
        g_print("open_item == NULL\n");
    }

    image_display = GTK_IMAGE(gtk_builder_get_object(builder, "sudoku"));
    if (image_display == NULL)
    {
        g_print("image_display == NULL\n");
    }
    panel_image = GTK_WIDGET(gtk_builder_get_object(builder, "panel"));
    if (panel_image== NULL)
    {
        g_print("panel_image == NULL\n");
    }
    event_box = GTK_WIDGET(gtk_builder_get_object(builder, "event_box"));
    if (event_box== NULL)
    {
        g_print("event_box == NULL\n");
    }


    GdkPixbuf *pixbuf = NULL;

    pixbuf = gdk_pixbuf_new_from_file ("imageBlanche.jpg", &error);
    if (!error)
    {
        GdkPixbuf *pixbuf_mini = NULL;

        pixbuf_mini = gdk_pixbuf_scale_simple (pixbuf,
                                               gdk_pixbuf_get_width (pixbuf) / 2,
                                               gdk_pixbuf_get_height (pixbuf) / 2,
                                               GDK_INTERP_NEAREST);

        gtk_image_set_from_pixbuf(GTK_IMAGE(image_display), pixbuf_mini);//image_display = gtk_image_new_from_pixbuf (pixbuf_mini);
        //find_fixed(GTK_WIDGET(main_window),NULL);
    }
    else
    {
        g_critical ("%s",error->message);
    }
    //////////////////////////////////////////////////////////
    //gtk_image_set_from_file(GTK_IMAGE(image_display), filename);

    gtk_widget_set_events(GTK_WIDGET(image_display), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(open_item), "activate", G_CALLBACK(open_image), main_window);

    // Connecter la fonction de rappel au signal "button-press-event"
    g_signal_connect(G_OBJECT(image_display), "button-press-event", G_CALLBACK(on_button_clicked), NULL);

    // Lance la boucle principale GTK
    gtk_widget_show_all(GTK_WIDGET(main_window));
/*
    // Réalise le widget avant de définir les événements
    if (!gtk_widget_get_realized(image_display)) {
        gtk_widget_realize(image_display);
    }

    // Active les événements de pression de bouton pour l'image
    gtk_widget_set_events(image_display, GDK_BUTTON_PRESS_MASK);*/

    // Lance la boucle principale GTK
    gtk_main();


    return 0;
}