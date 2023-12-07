#include <gtk/gtk.h>

GtkWidget *image_display;
GtkWindow * main_window;
GtkWidget *panel_image;

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

    gtk_widget_show_all(GTK_WIDGET(main_window));
}

/*
void open_image(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    gint response;

    // Crée une boîte de dialogue de sélection de fichiers
    dialog = gtk_file_chooser_dialog_new("Ouvrir une image",
                                         GTK_WINDOW(data),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "Annuler", GTK_RESPONSE_CANCEL,
                                         "Ouvrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Permet de sélectionner uniquement les fichiers image
    chooser = GTK_FILE_CHOOSER(dialog);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

    // Affiche la boîte de dialogue et récupère la réponse de l'utilisateur
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    char *filename;

    if (response == GTK_RESPONSE_ACCEPT) {

        GdkPixbuf *pixbuf = NULL;
        GError *error = NULL;
        //GtkWidget *image;

        // Récupère le chemin du fichier sélectionné
        filename = gtk_file_chooser_get_filename(chooser);

        // Affiche le chemin du fichier pour déboguer
        g_print("Chemin du fichier image : %s\n", filename);

        pixbuf = gdk_pixbuf_new_from_file (filename, &error);

        if (pixbuf != NULL) {
            GdkPixbuf *pixbuf_mini = gdk_pixbuf_scale_simple (pixbuf,
                                                              gdk_pixbuf_get_width (pixbuf) / 2,
                                                              gdk_pixbuf_get_height (pixbuf) / 2,
                                                              GDK_INTERP_NEAREST);

            // Crée un nouvel objet GtkImageMenuItem
            //GtkWidget *image_menu_item = gtk_image_menu_item_new_with_label("Image");

            // Crée un objet GtkImage avec le pixbuf redimensionné
            image_display = gtk_image_new_from_pixbuf(pixbuf_mini);
            g_print("image\n");

            // N'oublie pas de libérer la mémoire allouée pour le nom du fichier
            g_free(filename);
        } else {
            g_printerr("Erreur lors du chargement de l'image.\n");
        }

        // N'oublie pas de libérer la mémoire allouée pour le pixbuf
        g_object_unref(pixbuf);
    }

    if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
        g_print("Fichier sélectionné : %s\n", filename);
        g_free(filename);
    }

    // Détruit la boîte de dialogue
    gtk_widget_destroy(dialog);
    gtk_widget_show_all(GTK_WIDGET(main_window));
    gtk_main();
}*/



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

    image_display = GTK_WIDGET(gtk_builder_get_object(builder, "sudoku"));
    if (image_display == NULL)
    {
        g_print("image_display == NULL\n");
    }
    panel_image = GTK_WIDGET(gtk_builder_get_object(builder, "panel"));
    if (panel_image== NULL)
    {
        g_print("panel_image == NULL\n");
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


    g_signal_connect(G_OBJECT(open_item), "activate", G_CALLBACK(open_image), main_window);



     // Lance la boucle principale GTK
    gtk_widget_show_all(GTK_WIDGET(main_window));
    gtk_main();

    return 0;
}