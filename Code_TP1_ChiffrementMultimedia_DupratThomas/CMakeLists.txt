#Déclaration de la version de cmake nécessaire
cmake_minimum_required(VERSION 3.16)

#Déclaration du projet

project(TP1_Chiffrement)

#Génération de la liste des fichiers sources

file(

        GLOB_RECURSE

        source_files

        src/*

)

#Déclaration de l'exécutable

add_executable(

        TP1

        ${source_files}

)
