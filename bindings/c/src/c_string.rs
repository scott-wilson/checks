use std::{
    ffi::{c_char, CString},
    ptr::null_mut,
};

/// The OpenChecksString contains an owned pointer to a C style string.
///
/// # Safety
///
/// The pointer to the string must be destroyed with `openchecks_string_destroy`
/// once it is no longer needed. Also, the pointer must not be modified at all
/// by any functions not exposed by the validation library.
///
/// Internally, if a OpenChecksString is created, the system will create a copy
/// of the string being pointed to.
#[repr(C)]
pub struct OpenChecksString {
    /// The owned pointer to a string.
    ///
    /// # Safety
    ///
    /// This should not be modified at all outside of the validation library.
    /// Also, it should only be destroyed with `openchecks_string_destroy`.
    pub string: *mut c_char,
    /// Destroy the owned data.
    ///
    /// # Safety
    ///
    /// The destroy function should be called once at most.
    ///
    /// The destroy function should handle if the string pointer is null.
    pub destroy_fn: unsafe extern "C" fn(*mut Self) -> (),
}

impl Drop for OpenChecksString {
    fn drop(&mut self) {
        unsafe { (self.destroy_fn)(self) }
    }
}

impl OpenChecksString {
    pub(crate) fn new<T: AsRef<str>>(text: T) -> Self {
        unsafe extern "C" fn destroy_fn(string: *mut OpenChecksString) {
            if !(*string).string.is_null() {
                unsafe { drop(CString::from_raw((*string).string)) };
                (*string).string = null_mut();
            }
        }

        Self {
            string: match CString::new(text.as_ref()) {
                Ok(r) => r.into_raw(),
                Err(_) => null_mut(),
            },
            destroy_fn,
        }
    }
}

/// Destroy a string pointer.
///
/// # Safety
///
/// The pointer must not be null, and must not already have been destroyed (AKA:
/// double free). Once the destroy function is called, all pointers to the
/// string are invalid.
#[no_mangle]
pub unsafe extern "C" fn openchecks_string_destroy(string: *mut OpenChecksString) {
    unsafe { string.drop_in_place() }
}

/// The OpenChecksStringView creates a borrowed pointer to a C style string.
///
/// # Safety
///
/// The pointer must not outlive the container that owns the string. Also, the
/// pointer should not be null, but that is not a strict requirement.
#[repr(C)]
pub struct OpenChecksStringView {
    /// The borrowed pointer to a string.
    ///
    /// # Safety
    ///
    /// The string must not outlive the container that owns it.
    pub string: *const c_char,
}

impl OpenChecksStringView {
    pub(crate) fn from_ptr(string: *const c_char) -> Self {
        Self { string }
    }
}
