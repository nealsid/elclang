(module-load "libelclang.dylib")
(elclang-initialize-build-tree "/Users/nealsid/src/github/llvm-project-build")
(elclang-file-visited "/Users/nealsid/src/github/llvm-project/lldb/source/Host/common/Editline.cpp")

(defface elclang-clang-cursor-extent-around-point '((t :background "LightPink1")) "Face used for current cursor extent overlay in elclang.")

(defun elclang-get-cursor-extent-for-cursor-under-point ()
  (interactive)
  (let ((line-number (line-number-at-pos))
        (column-number (current-column)))
    (elclang-cursor-extent-for-point line-number column-number (buffer-file-name))))
