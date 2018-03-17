/* -*- mode: C; c-basic-offset: 4; -*- */
/* ex: set shiftwidth=4 tabstop=4 expandtab: */
/*
 * Copyright (c) 2015, Rice University
 * All rights reserved.
 *
 * Author(s): Neil T. Dantam <ntd@rice.edu>
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of copyright holder the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef AMINO_RX_SCENE_PLUGIN_H
#define AMINO_RX_SCENE_PLUGIN_H

/**
 * @file scene_plugin.h
 * @brief Dynamically load compiled scene graphs.
 *
 *
 * @sa @ref scenecompiler
 */

/**
 * Type signature of compiled scene graph functions.
 */
typedef struct aa_rx_sg *(*aa_rx_dl_sg_fun)(struct aa_rx_sg *sg, const char *root);

/**
 * Type signature of compiled mesh functions.
 */
typedef struct aa_rx_mesh *(*aa_rx_dl_mesh_fun)(void);

/**
 * Dynamically load a compiled mesh
 */
AA_API struct aa_rx_mesh *
aa_rx_dl_mesh( const char *filename, const char *name );


/**
 * Dynamically load a compiled scene graph.
 *
 * This function dynamically loads the scene graph plugin (via
 * dlopen()), looks up the symbol for the scene graph load function
 * (via dlsym()), and calls the function to load the scene graph.
 *
 * @param filename   The name of the shared object, passed
 *                   directly as the first parameter to dlopen().
 *
 * @param name       The name of the scene graph, as specified in the
 *                   prior call to the scene graph compiler.  Used to
 *                   construct the symbol argument for dlsym().
 *
 * @param scenegraph An initial scenegraph to which the loaded
 *                   scenegraph will be added, or NULL.
 *
 * @sa aa_rx_dl_sg_at
 *
 */
AA_API struct aa_rx_sg *
aa_rx_dl_sg( const char *filename, const char *name,
             struct aa_rx_sg *scenegraph);

/**
 * Dynamically load a compiled scene graph under a root frame.
 *
 * This function dynamically loads the scene graph plugin (via
 * dlopen()), looks up the symbol for the scene graph load function
 * (via dlsym()), and calls the function to load the scene graph.
 *
 * @param filename   The name of the shared object, passed
 *                   directly as the first parameter to dlopen().
 *
 * @param name       The name of the scene graph, as specified in the
 *                   prior call to the scene graph compiler.  Used to
 *                   construct the symbol argument for dlsym().
 *
 * @param scenegraph An initial scenegraph to which the loaded
 *                   scenegraph will be added, or NULL.
 *
 * @param root       The root frame under which the new scenegraph is
 *                   loaded.
 *
 * @sa aa_rx_dl_sg
 *
 */
AA_API struct aa_rx_sg *
aa_rx_dl_sg_at ( const char *filename, const char *name,
                 struct aa_rx_sg *scenegraph, const char *root );
#endif /*AMINO_RX_SCENE_PLUGIN_H*/
