(module-load "/Users/nealsid/src/github/elclang/build/libelclang.dylib")
(elclang-initialize-build-tree "/Users/nealsid/src/github/llvm-project-build")
(find-file-other-window "/Users/nealsid/src/github/llvm-project/lldb/Source/Host/Common/Editline.cpp")
(elclang-file-visited "/Users/nealsid/src/github/llvm-project/lldb/Source/Host/Common/Editline.cpp")
;;(elclang-dump-ast "/Users/nealsid/src/github/llvm-project/lldb/Source/Host/Common/Editline.cpp")
(goto-line 317)
(right-char 4)
(setq output-vector (make-vector 4 0))
(elclang-cursor-extent-for-point (line-number-at-pos) (current-column) (buffer-file-name) output-vector)

(defface elclang-clang-cursor-extent-around-point '((t :background "LightPink1")) "Face used for current cursor extent overlay in elclang.")

output-vector
(aref output-vector 1)
(goto-line (aref output-vector 0))
(move-to-column (aref output-vector 1))
(setq overlay-start (point))
(goto-line (aref output-vector 2))
(move-to-column (aref output-vector 3))
(setq overlay-end (point))
(setq cursor-overlay (make-overlay overlay-start overlay-end))
(overlay-put cursor-overlay 'face 'elclang-clang-cursor-extent-around-point)

(defun elclang-add-overlay-timer-function ()
  (remove-overlays nil nil 'elclang-overlay t)
  (let ((output-vector (make-vector 4 0)))
    (elclang-cursor-extent-for-point (line-number-at-pos) (current-column) (buffer-file-name) output-vector)
    (let* ((overlay-begin (point-for-line-and-col (aref output-vector 0)
						  (aref output-vector 1)))
	   (overlay-end (point-for-line-and-col (aref output-vector 2)
						(aref output-vector 3)))
	   (cursor-overlay (make-overlay overlay-begin overlay-end)))
      (overlay-put cursor-overlay 'face 'elclang-clang-cursor-extent-around-point)
      (overlay-put cursor-overlay 'elclang-overlay t))))


(setq elclang-decorate-buffer-timer (run-at-time .1 .1 'elclang-add-overlay-timer-function))
(cancel-timer elclang-decorate-buffer-timer)
(defun point-for-line-and-col (line col)
  (save-excursion
    (goto-line line)
    (move-to-column col)
    (point)))

output-vector
