#include <gtk/gtk.h>

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
    gtk_file_chooser_set_filter(chooser, NULL);


    // Affiche la boîte de dialogue et récupère la réponse de l'utilisateur
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkImage *image;

        // Récupère le chemin du fichier sélectionné
        filename = gtk_file_chooser_get_filename(chooser);

        // Charge l'image dans un widget GtkImage
        image = GTK_IMAGE(gtk_image_new_from_file(filename));

        // Affiche l'image dans une fenêtre ou une boîte de dialogue, selon tes besoins
        // Par exemple, si tu as une fenêtre principale appelée "main_window":
        // gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(image));

        // N'oublie pas de libérer la mémoire allouée pour le nom du fichier
        g_free(filename);
    }

    // Détruit la boîte de dialogue
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {

    // Initialise GTK
    gtk_init(NULL,NULL);
    GtkBuilder* builder = gtk_builder_new ();

    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "interfaceG.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    GtkWidget *open_item = GTK_WIDGET(gtk_builder_get_object(builder, "open_image"));


    g_signal_connect(G_OBJECT(open_item), "activate", G_CALLBACK(open_image), window);

     // Lance la boucle principale GTK
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    return 0;
}
